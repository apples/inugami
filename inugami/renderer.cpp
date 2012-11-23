#include "renderer.h"

#include "loaders.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace Inugami {

const unsigned int Renderer::PrintBuffer::itemDataSizeIncrement = 4;

Renderer::RenderParams::RenderParams() :
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

Renderer::RenderParams::Mode2D_t::Mode2D_t() :
    width(2.0), height(2.0),
    nearClip(-1.0), farClip(1.0)
{
    //Not much else to do here...
}

bool Renderer::TexParams::operator==(const TexParams &in) const
{
    return (smooth==in.smooth && clamp==in.clamp);
}

bool Renderer::TexParams::operator<(const TexParams &in) const
{
    if (!smooth && in.smooth) return true;
    if (smooth == in.smooth && !clamp && in.clamp) return true;
    return false;
}

bool Renderer::TextureIndex::operator<(const TextureIndex &in) const
{
    if (fileName < in.fileName) return true;
    if (fileName == in.fileName && params < in.params) return true;
    return false;
}

Renderer::Renderer(const RenderParams &params) :
    printer(this),
    windowTitle("Inugami"),
    windowTitleShowFPS(false)
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

    //Generate the blank texture
    unsigned int whitePixel = 0xffffffff;
    glGenTextures(1, &blankTexture);
    glBindTexture(GL_TEXTURE_2D, blankTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel);

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

    nullTex.id = nullptr;
}

Renderer::~Renderer()
{
    glfwTerminate();
}


void Renderer::beginFrame()
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

void Renderer::endFrame()
{
    glfwSwapBuffers();
}

void Renderer::drawString(const char *text, bool pushpop)
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

double Renderer::getInstantFrameRate()
{
    return 1.0/(glfwGetTime() - frameStartTime);
}

double Renderer::getAverageFrameRate()
{
    double sum = 0;
    std::list<double>::iterator i;
    for (i=frameRateStack.begin(); i!=frameRateStack.end(); ++i)
    {
        sum += *i;
    }
    return sum/10.0;
}

bool Renderer::setMode(RenderMode m, RenderFace s)
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
    case RM_3D:
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

    case RM_2D:
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

    case RM_INTERFACE:
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

Renderer::Texture Renderer::loadTexture(const char *fileName, const TexParams &p)
{
    std::string foo(fileName);
    return loadTexture(foo, p);
}

Renderer::Texture Renderer::loadTexture(const std::string &fileName, const TexParams &p)
{
    TextureIndex ti;
    ti.fileName = fileName;
    ti.params = p;

    auto i = textures.find(ti);
    if (i != textures.end())
    {
        ++i->second.users;
        Texture rval;
        rval.width = i->second.width;
        rval.height = i->second.height;
        rval.id = &i->second.id;
        return rval;
    }

    std::vector<char> data;
    if (!loadImageFromFile(fileName, data))
    {
        Texture rval;
        rval.id = nullptr;
        return rval;
    }

    glGenTextures(1, &textures[ti].id);
    textures[ti].users = 1;

    glBindTexture(GL_TEXTURE_2D, textures[ti].id);

    if (p.smooth)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    if (p.clamp)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *reinterpret_cast<int*>(&data[0]), *reinterpret_cast<int*>(&data[4]),
        0, GL_RGBA, GL_UNSIGNED_BYTE, &data[8]
    );

    Texture rval;
    rval.width = *reinterpret_cast<int*>(&data[0]);
    rval.height = *reinterpret_cast<int*>(&data[sizeof(int)]);
    rval.id = &textures[ti].id;

    textures[ti].width = rval.width;
    textures[ti].height = rval.height;

    return rval;
}

void Renderer::setTexture(const Texture &in)
{
    if (in.id == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, blankTexture);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *in.id);
    }
}

void Renderer::reloadTextures()
{
    for (auto i : textures)
    {
        std::vector<char> data;
        loadImageFromFile(i.first.fileName, data);
        glBindTexture(GL_TEXTURE_2D, i.second.id);

        if (i.first.params.smooth)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        if (i.first.params.clamp)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *reinterpret_cast<int*>(&data[0]), *reinterpret_cast<int*>(&data[4]),
            0, GL_RGBA, GL_UNSIGNED_BYTE, &data[8]
        );

    }
}

void Renderer::dropTexture(const Texture &in)
{
    auto i = textures.begin();
    while (i != textures.end())
    {
        if (i->second.id == *in.id)
        {
            if (--i->second.users == 0) textures.erase(i);
            break;
        }
        ++i;
    }
}

Mesh* Renderer::loadMesh(const char *fileName)
{
    std::string foo(fileName);
    return loadMesh(foo);
}

Mesh* Renderer::loadMesh(const std::string &fileName)
{
    auto i = meshes.find(fileName);
    if (i != meshes.end())
    {
        ++i->second.users;
        return &i->second.mesh;
    }

    meshes[fileName].users = 1;
    if (!loadObjFromFile(fileName, meshes[fileName].mesh))
    {
        meshes.erase(meshes.find(fileName));
        return nullptr;
    }

    return &meshes[fileName].mesh.init();
}

void Renderer::reloadMeshes()
{
    for (auto i : meshes)
    {
        loadObjFromFile(i.first, i.second.mesh);
        i.second.mesh.init();
    }
}

void Renderer::dropMesh(Mesh* target)
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

void Renderer::setWindowTitle(const char *text, bool showFPS)
{
    windowTitle = text;
    windowTitleShowFPS = showFPS;
    glfwSetWindowTitle(text);
}

const Renderer::RenderParams& Renderer::getParams()
{
    return rparams;
}

void Renderer::dumpState(std::ostream &&out)
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

    {
        out << "Textures:\n";
        int i=0;
        for (auto p : textures)
        {
            out << "  Index: " << i++ << "\n";
            out << "    Name:       \"" << p.first.fileName << "\"\n";
            out << "    Dimensions: " << p.second.width << "x" << p.second.height << "\n";
            out << "    Params:\n";
            out << "      Smooth:   " << bool2text(p.first.params.smooth) << "\n";
            out << "      Clamp:    " << bool2text(p.first.params.clamp) << "\n";
            out << "    OpenGL ID:  " << p.second.id << "\n";
            out << "    Users:      " << p.second.users << "\n";
        }
    }

    {
        out << "Meshes:\n";
        int i=0;
        for (auto p : meshes)
        {
            out << "  Index: " << i++ << "\n";
            out << "    Name:           \"" << p.first << "\"\n";
            out << "    Vertices:       " << p.second.mesh.vertices.size() << "\n";
            out << "    Triangles:      " << p.second.mesh.triangles.size() << "\n";
            out << "    Initialized :   " << bool2text(p.second.mesh.initted) << "\n";
            if (p.second.mesh.initted)
            {
                out << "    Vertex Buffer:  " << p.second.mesh.vbo << "\n";
                out << "    Element Buffer: " << p.second.mesh.ele << "\n";
            }
        }
    }

    out.flush();
}

Renderer::PrintBuffer::PrintBuffer(Renderer* p) :
    buffer(0), parent(p)
{}

void Renderer::PrintBuffer::print()
{
    glPushMatrix();
    for (auto i : buffer)
    {
        switch (i.t)
        {
        case PrintItem::STRING:
            parent->drawString(&i.data[0], false);
            break;
        case PrintItem::COLOR:
            glColor4ubv(reinterpret_cast<GLubyte*>(&i.data[0]));
            break;
        }
    }
    glPopMatrix();
    buffer.clear();
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(PrintItem in)
{
    buffer.push_back(Item());
    Item &back = buffer.back();
    back.t = in;
    back.dataSize = 0;

    switch (in)
    {
    case COLOR:
        back.data.resize(4, 0xff);
        back.dataMax = 4;
        break;
    case STRING:
        back.data.resize(itemDataSizeIncrement);
        back.data[0] = 0;
        back.dataMax = itemDataSizeIncrement;
        break;
    }

    return *this;
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(char in)
{
    Item &back = buffer.back();
    if (back.t == PrintItem::COLOR)
    {
        if (back.dataSize > 3) return *this;
        back.data[back.dataSize] = in;
        ++back.dataSize;
        return *this;
    }
    std::string s;
    s = in;
    return (*this) << s;
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(int in)
{
    Item &back = buffer.back();
    if (back.t == PrintItem::COLOR)
    {
        if (back.dataSize > 3) return *this;
        back.data[back.dataSize] = static_cast<char>(in);
        ++back.dataSize;
        return *this;
    }
    std::stringstream ss;
    ss << in;
    return (*this) << ss;
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(float in)
{
    Item &back = buffer.back();
    if (back.t == PrintItem::COLOR)
    {
        if (back.dataSize > 3) return *this;
        back.data[back.dataSize] = static_cast<char>(in*255.0);
        ++back.dataSize;
        return *this;
    }
    std::stringstream ss;
    ss << in;
    return (*this) << ss;
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(std::stringstream &in)
{
    std::string s = in.str();
    return (*this) << s;
}

Renderer::PrintBuffer &Renderer::PrintBuffer::operator<<(std::string &in)
{
    Item *back = &buffer.back();
    if (back->t != PrintItem::STRING || buffer.size() < 1)
    {
        (*this) << PrintItem::STRING;
        back = &buffer.back();
    }

    unsigned int dataNeeded = back->dataSize + in.size();

    //Expand if necessary
    if (dataNeeded+1 > back->dataMax)
    {
        while (dataNeeded+1 > back->dataMax) back->dataMax += itemDataSizeIncrement;
        back->data.resize(back->dataMax);
    }

    //Add new data
    for (unsigned int i=0; i<in.size(); ++i) back->data[i+back->dataSize] = in[i];
    back->dataSize = dataNeeded;
    back->data[dataNeeded] = 0;

    return *this;
}

} // namespace Inugami
