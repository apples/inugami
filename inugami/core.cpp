/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "core.h"

#include "loaders.h"
#include "math.h"
#include "opengl.h"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace Inugami {

Core::RenderParams::RenderParams() :
    width(800), height(600),
    fullscreen(false),
    vsync(false),
    fsaaSamples(4)
{}

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

    glfwWindowHint(GLFW_FSAA_SAMPLES, rparams.fsaaSamples);

    window = glfwCreateWindow
    (
        rparams.width, rparams.height,
        (rparams.fullscreen)? GLFW_FULLSCREEN : GLFW_WINDOWED,
        windowTitle.c_str(), 0
    );

    if (!window)
    {
        throw std::runtime_error("Failed to open window.");
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW.");
    }

    if (rparams.vsync) glfwSwapInterval(1);
    else glfwSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glClearDepth(1);
    glDepthFunc(GL_LEQUAL);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for (int i=0; i<MAXCALLBACKS; ++i)
    {
        callbacks[i].func = nullptr;
        callbacks[i].freq = 0.0;
        callbacks[i].wait = 0.0;
        callbacks[i].last = 0.0;
    }

    createDefaultShader();

    iface = new Interface(window);
}

Core::~Core()
{
    delete defaultShader;
    delete iface;

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Core::beginFrame()
{
    glfwMakeContextCurrent(window);

    *frStackIterator = getInstantFrameRate();
    ++frStackIterator;
    if (frStackIterator == frameRateStack.end())
    {
        frStackIterator = frameRateStack.begin();
    }
    frameStartTime = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getShader()->bind();

    //Title
    if (windowTitleShowFPS)
    {
        std::stringstream ss;
        ss << windowTitle << " (" << std::fixed << std::setprecision(2) << getAverageFrameRate() << " FPS)";
        glfwSetWindowTitle(window, ss.str().c_str());
    }
}

void Core::endFrame()
{
    glfwSwapBuffers(window);
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

void Core::applyCam(const Camera& in)
{
    //Cull faces
    if (in.cullFaces)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    if (in.depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    getShader()->setUniform("projectionMatrix", in.getProjection());
    getShader()->setUniform("viewMatrix", in.getView());
    getShader()->setUniform("modelMatrix", ::glm::mat4(1.f));
}

void Core::modelMatrix(const Mat4& in)
{
    getShader()->setUniform("modelMatrix", in);
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
    glfwSetWindowTitle(window, text);
}

const Core::RenderParams& Core::getParams()
{
    return rparams;
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

Interface* Core::getInterface()
{
    return iface;
}

const Shader* Core::getShader()
{
    if (!customShader) return defaultShader;
    return customShader;
}

void Core::setShader(Shader* in)
{
    customShader = in;
}

int Core::getWindowParam(int param)
{
    return glfwGetWindowParam(window, param);
}

void Core::createDefaultShader()
{
    Shader::Program program;
    program[Shader::Type::VERT] =
        "#version 400\n"
        "layout (location = 0) in vec3 VertexPosition;\n"
        "layout (location = 1) in vec3 VertexNormal;\n"
        "layout (location = 2) in vec2 VertexTexCoord;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 modelMatrix;\n"
        "out vec3 Position;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    TexCoord = VertexTexCoord;\n"
        "    Normal = normalize(VertexNormal);\n"
        "    Position = VertexPosition/5;\n"
        "    mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;\n"
        "    gl_Position = modelViewProjectionMatrix * vec4(VertexPosition,1.0);\n"
        "}\n"
    ;
    program[Shader::Type::FRAG] =
        "#version 400\n"
        "in vec3 Position;\n"
        "in vec3 Normal;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D Tex1;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec4 texColor = texture( Tex1, TexCoord );\n"
        "    FragColor = texColor;\n"
        "}\n"
    ;
    defaultShader = new Shader(program);
}

} // namespace Inugami
