#include "core.h"

#include "loaders.h"
#include "math.h"
#include "opengl.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace Inugami {

Core::RenderParams::RenderParams() :
    width(800), height(600),
    fullscreen(false),
    vsync(false),
    fov(90.0),
    nearClip(0.1), farClip(100.0),
    fsaaSamples(4),
    mode2D()
{
    //Not much else to do here...
}

Core::RenderParams::Mode2D_t::Mode2D_t() :
    width(2.0), height(2.0),
    nearClip(-1.0), farClip(1.0)
{
    //Not much else to do here...
}

Core::Core(const RenderParams &params) :
    windowTitle("Inugami"),
    windowTitleShowFPS(false),
    running(false), callbackCount(0)
{
    rparams = params;
    aspectRatio = static_cast<double>(rparams.width)/static_cast<double>(rparams.height);

    frameRateStack.resize(10, double(0.0));
    frStackIterator = frameRateStack.begin();

    glfwInit();

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, rparams.fsaaSamples);

    if (glfwOpenWindow(rparams.width, rparams.height, 8, 8, 8, 8, 8, 0, (rparams.fullscreen)? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE) throw;
    if (glewInit() != GLEW_OK) throw;

    glfwSetWindowTitle(windowTitle.c_str());

    if (rparams.vsync) glfwSwapInterval(1);
    else glfwSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //Generate lists for fonts
    fontLists = glGenLists(256);
    for (unsigned int i=0; i<256; ++i)
    {
        glNewList(fontLists+i, GL_COMPILE);
        float u=0.0, v=0.0;
        for (unsigned int j=0; j<i%16; ++j) u+=0.0625;
        for (unsigned int j=0; j<i/16; ++j) v+=0.0625;
        glBegin(GL_QUADS);
            glTexCoord2f(u, v);
            glVertex2f(0.0, 0.0);
            glTexCoord2f(u, v+0.0625);
            glVertex2f(0.0, 1.0);
            glTexCoord2f(u+0.0625, v+0.0625);
            glVertex2f(1.0, 1.0);
            glTexCoord2f(u+0.0625, v);
            glVertex2f(1.0, 0.0);
        glEnd();
        glEndList();
    }

    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        callbacks[i].func = nullptr;
        callbacks[i].freq = 0.0;
        callbacks[i].wait = 0.0;
        callbacks[i].last = 0.0;
    }
}

Core::~Core()
{
    glfwTerminate();
}


void Core::beginFrame()
{
    *frStackIterator = getInstantFrameRate();
    ++frStackIterator;
    if (frStackIterator == frameRateStack.end())
    {
        frStackIterator = frameRateStack.begin();
    }
    frameStartTime = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //Title
    if (windowTitleShowFPS)
    {
        std::stringstream ss;
        ss << windowTitle << " (" << std::fixed << std::setprecision(2) << getAverageFrameRate() << " FPS)";
        glfwSetWindowTitle(ss.str().c_str());
    }
}

void Core::endFrame()
{
    glfwSwapBuffers();
}

void Core::drawString(const char *text, bool pushpop)
{
    if (pushpop) glPushMatrix();
    float x = 0.0f;
    unsigned int i=0;
    while (text[i]!=0 && i<256)
    {
        if (text[i] == '\n')
        {
            glTranslatef(-x, 1.0f, 0.0f);
            x = 0.0f;
            ++i;
            continue;
        }
        glCallList(fontLists+text[i]);
        glTranslatef(1.0f, 0.0f, 0.0f);
        x += 1.0f;
        ++i;
    }
    if (pushpop) glPopMatrix();
}

double Core::getInstantFrameRate()
{
    return 1.0/(glfwGetTime() - frameStartTime);
}

double Core::getAverageFrameRate()
{
    double sum = 0;
    std::list<double>::iterator i;
    for (i=frameRateStack.begin(); i!=frameRateStack.end(); ++i)
    {
        sum += *i;
    }
    return sum/10.0;
}

bool Core::setMode(RenderMode m, RenderFace s)
{
    //Cull faces
    if (s == RF_BOTH)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        if (s == RF_BACK) glCullFace(GL_FRONT);
        else glCullFace(GL_BACK);
    }

    switch (m)
    {
    case RenderMode::PERSPECTIVE:
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(rparams.fov, aspectRatio, rparams.nearClip, rparams.farClip);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_CW);
        glEnable(GL_CULL_FACE);
        //glLoadMatrixf(cam.getMatrix().transpose().data[0].data);
        break;

    case RenderMode::ORTHOGONAL:
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-rparams.mode2D.width*0.5, rparams.mode2D.width*0.5,
            -rparams.mode2D.height*0.5, rparams.mode2D.height*0.5,
            rparams.mode2D.nearClip, rparams.mode2D.farClip
        );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glEnable(GL_DEPTH_TEST);
        break;

    case RenderMode::INTERFACE:
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, rparams.width, rparams.height, 0.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        break;
    }

    return true;
}

Mesh* Core::loadMesh(const char *filename)
{
    std::string foo(filename);
    return loadMesh(foo);
}

Mesh* Core::loadMesh(const std::string &filename)
{
    auto i = meshes.find(filename);
    if (i != meshes.end())
    {
        ++i->second.users;
        return &i->second.mesh;
    }

    meshes[filename].users = 1;
    if (!loadObjFromFile(filename, meshes[filename].mesh))
    {
        meshes.erase(meshes.find(filename));
        return nullptr;
    }

    return &meshes[filename].mesh.init();
}

void Core::reloadMeshes()
{
    for (auto i : meshes)
    {
        loadObjFromFile(i.first, i.second.mesh);
        i.second.mesh.init();
    }
}

void Core::dropMesh(Mesh* target)
{
    auto i = meshes.begin();
    while (i != meshes.end())
    {
        if (&i->second.mesh == target)
        {
            if (--i->second.users == 0) meshes.erase(i);
            break;
        }
        ++i;
    }
}

void Core::setWindowTitle(const char *text, bool showFPS)
{
    windowTitle = text;
    windowTitleShowFPS = showFPS;
    glfwSetWindowTitle(text);
}

const Core::RenderParams& Core::getParams()
{
    return rparams;
}

void Core::dumpState(std::ostream &&out)
{
    auto bool2text = [](bool in){return in?"True":"False";};

    out << "Address: " << this << "\n";
    out << "Current average framerate: " << getAverageFrameRate() << "\n";

    const RenderParams &rp = getParams();
    out << "Render params:\n";
    out << "  Dimensions: " << rp.width << "x" << rp.height << "\n";
    out << "  Fullscreen: " << bool2text(rp.fullscreen) << "\n";
    out << "  VSync:      " << bool2text(rp.vsync) << "\n";
    out << "  FOV:        " << rp.fov << "\n";
    out << "  Clipping:   " << rp.nearClip << ":" << rp.farClip << "\n";
    out << "  FSAA:       " << rp.fsaaSamples << "\n";
    out << "  Mode 2D:\n";
    out << "    Dimensions: " << rp.mode2D.width << "x" << rp.mode2D.height << "\n";
    out << "    Clipping:   " << rp.mode2D.nearClip << ":" << rp.mode2D.farClip << "\n";

//    {
//        out << "Textures:\n";
//        int i=0;
//        for (auto p : textures)
//        {
//            out << "  Index: " << i++ << "\n";
//            out << "    Name:       \"" << p.first.filename << "\"\n";
//            out << "    Dimensions: " << p.second.width << "x" << p.second.height << "\n";
//            out << "    Params:\n";
//            out << "      Smooth:   " << bool2text(p.first.params.smooth) << "\n";
//            out << "      Clamp:    " << bool2text(p.first.params.clamp) << "\n";
//            out << "    OpenGL ID:  " << p.second.id << "\n";
//            out << "    Users:      " << p.second.users << "\n";
//        }
//    }

//    {
//        out << "Meshes:\n";
//        int i=0;
//        for (auto p : meshes)
//        {
//            out << "  Index: " << i++ << "\n";
//            out << "    Name:           \"" << p.first << "\"\n";
//            out << "    Vertices:       " << p.second.mesh.vertices.size() << "\n";
//            out << "    Triangles:      " << p.second.mesh.triangles.size() << "\n";
//            out << "    Initialized :   " << bool2text(p.second.mesh.initted) << "\n";
//            if (p.second.mesh.initted)
//            {
//                out << "    Vertex Buffer:  " << p.second.mesh.vbo << "\n";
//                out << "    Element Buffer: " << p.second.mesh.ele << "\n";
//            }
//        }
//    }

    out.flush();
}

bool Core::addCallback(void (*func)(), double freq)
{
    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        if (i == callbackCount)
        {
            callbacks[i].func = func;
            callbacks[i].freq = freq;
            callbacks[i].wait = 0.0;
            callbacks[i].last = glfwGetTime();
            ++callbackCount;
            return true;
        }
        if (callbacks[i].func == func)
        {
            callbacks[i].freq = freq;
            callbacks[i].wait = 0.0;
            callbacks[i].last = glfwGetTime();
            return true;
        }
    }
    return false;
}

void Core::dropCallback(void (*func)())
{
    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        if (i == callbackCount)
        {
            return;
        }
        if (callbacks[i].func == func)
        {
            for (int j=i; j<callbackCount-1; ++j)
            {
                callbacks[j].func = callbacks[j+1].func;
                callbacks[j].freq = callbacks[j+1].freq;
                callbacks[j].wait = callbacks[j+1].wait;
                callbacks[j].last = callbacks[j+1].last;
            }
            callbacks[callbackCount-1].func = nullptr;
            return;
        }
    }
}

int Core::go()
{
    running = true;

    double currentTime = 0.0;
    double deltaTime = 0.0;

    while (running) {
        for (int i=0; i<callbackCount; ++i)
        {
            currentTime = glfwGetTime();
            deltaTime = currentTime - callbacks[i].last;
            callbacks[i].last = currentTime;

            if (callbacks[i].freq < 0.0)
            {
                callbacks[i].func();
                continue;
            }

            callbacks[i].wait += deltaTime * callbacks[i].freq;
            if (callbacks[i].wait >= 1.0)
            {
                callbacks[i].func();
                callbacks[i].wait = 0.0;
            }

            if (!running) break;
        }
    }

    return 0;
}

} // namespace Inugami
