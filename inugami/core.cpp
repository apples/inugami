/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "core.hpp"

#include "camera.hpp"
#include "exception.hpp"
#include "interface.hpp"
#include "loaders.hpp"
#include "shader.hpp"
#include "sharedbank.hpp"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace Inugami {

int Core::numCores = 0;

class CoreException : public Exception
{
public:
    CoreException(Core* c, std::string error) :
        core(c), err(error)
    {}

    const char* what() const noexcept override
    {
        std::string rval;
        rval += "Core Exception: ";
        rval += hexify(core);
        rval += "; ";
        rval += err;
        return rval.c_str();
    }

    Core* core;
    std::string err;
};

Core::RenderParams::RenderParams() :
    width(800), height(600),
    fullscreen(false),
    vsync(false),
    fsaaSamples(0)
{}

Core::Core(const RenderParams &params) :
    running(false),

    frameStartTime(0.f),
    frameRateStack(10, 0.0),
    frStackIterator(frameRateStack.begin()),

    rparams(params),
    aspectRatio(double(rparams.width)/double(rparams.height)),

    windowTitle("Inugami"),
    windowTitleShowFPS(false),
    window(nullptr),

    shader(nullptr),

    banks(new SharedBank)
{
    if (numCores == 0) glfwInit();

    glfwWindowHint(GLFW_SAMPLES, rparams.fsaaSamples);

    window = glfwCreateWindow
    (
        rparams.width, rparams.height,
        windowTitle.c_str(),
        (rparams.fullscreen)? nullptr : nullptr, //TODO detect monitors
        nullptr
    );

    if (!window)
    {
        throw CoreException(this, "Failed to open window.");
    }

    activate();

    if (numCores == 0) if (glewInit() != GLEW_OK)
    {
        throw CoreException(this, "Failed to initialize GLEW.");
    }

    if (rparams.vsync) glfwSwapInterval(1);
    else glfwSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);

    glClearColor(1.0, 0.0, 1.0, 0.0);

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

    createDefaultShader();

    iface = new Interface(window);

    ++numCores;
}

Core::~Core()
{
    activate();

    delete iface;

    glfwDestroyWindow(window);

    if (--banks->users == 0) delete banks;

    if (--numCores == 0)
    {
        glfwTerminate();
    }
}

void Core::activate() const
{
    glfwMakeContextCurrent(window);
}

void Core::deactivate() const
{
    glfwMakeContextCurrent(0);
}

void Core::beginFrame()
{
    activate();

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

double Core::getInstantFrameRate() const
{
    return 1.0/(glfwGetTime() - frameStartTime);
}

double Core::getAverageFrameRate() const
{
    double sum = 0;
    for (auto& i : frameRateStack)
    {
        sum += i;
    }
    return sum/10.0;
}

void Core::applyCam(const Camera& in)
{
    activate();

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
    activate();
    getShader()->setUniform("modelMatrix", in);
}

void Core::setWindowTitle(const char *text, bool showFPS)
{
    windowTitle = text;
    windowTitleShowFPS = showFPS;
    glfwSetWindowTitle(window, text);
}

const Core::RenderParams& Core::getParams() const
{
    return rparams;
}

void Core::addCallback(std::function<void()> func, double freq)
{
    callbacks.push_back({func, freq, glfwGetTime()});
}

void Core::clearCallbacks()
{
    callbacks.clear();
}

int Core::go()
{
    running = true;

    double currentTime = 0.0;
    double deltaTime = 0.0;

    while (running)
    {
        for (auto& cb : callbacks)
        {
            if (cb.freq < 0.0)
            {
                cb.func();
                continue;
            }

            currentTime = glfwGetTime();
            deltaTime = (currentTime - cb.last)*cb.freq;

            if (deltaTime >= 1.0)
            {
                cb.last = currentTime;
                cb.func();
            }

            if (!running) break;
        }
    }

    return 0;
}

const Shader* Core::getShader() const
{
    if (!shader) return banks->shader;
    return shader;
}

void Core::setShader(Shader* in)
{
    shader = in;
}

int Core::getWindowParam(int param) const
{
    return glfwGetWindowParam(window, param);
}

void Core::createDefaultShader()
{
    if (!banks->shader)
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
        banks->shader = new Shader(program);
    }
}

std::string Core::getDiagnostic() const
{
    std::stringstream rval;

    rval << "-- BEGIN CORE DIAGNOSIS --\n";

    rval << "\tCore location:  " << hexify(this) << "\n";
    rval << "\tIface location: " << hexify((Interface*)iface) << "\n";

    rval << "\tRunning:        " << running << "\n";
    if (running) rval << "\t\tCurrent Framerate: " << getAverageFrameRate() << "\n";

    rval << "\tRender Paramters:\n";
    rval << "\t\tDimensions: " << rparams.width << "x" << rparams.height << "\n";
    rval << "\t\tFullscreen: " << rparams.fullscreen << "\n";
    rval << "\t\tVSync:      " << rparams.vsync << "\n";
    rval << "\t\tFSAA:       " << rparams.fsaaSamples << "\n";

    rval << "\tMeshes:\n";
    for (auto& p : banks->meshBank)
    {
        rval << "\t\tName: \"" << p.first.name << "\"\n";
        rval << "\t\t\tVertices:    " << p.second.vertices.size() << "\n";
        rval << "\t\t\tTriangles:   " << p.second.triangles.size() << "\n";
        rval << "\t\t\tInitialized: " << p.second.initted << "\n";
        rval << "\t\t\tVAO ID:      " << hexify(p.second.vao) << "\n";
        rval << "\t\t\tVBO ID:      " << hexify(p.second.vbo) << "\n";
        rval << "\t\t\tELE ID:      " << hexify(p.second.ele) << "\n";
        rval << "\t\t\tUsers:       " << p.second.users << "\n";
    }

    rval << "\tTextures:\n";
    for (auto& p : banks->textureBank)
    {
        rval << "\t\tName: \"" << p.first.name << "\"";
            if (p.first.clamp)  rval << " [CLAMP]";
            if (p.first.smooth) rval << " [SMOOTH]";
        rval << "\n";
        rval << "\t\t\tGL ID:      " << hexify(p.second.id) << "\n";
        rval << "\t\t\tDimensions: " << p.second.width << "x" << p.second.height << "\n";
        rval << "\t\t\tUsers:      " << p.second.users << "\n";
    }

    rval << "\tSpritesheets:\n";
    for (auto& p : banks->spritesheetBank)
    {
        rval << "\t\tDimensions:";
            rval << " [" << p.first.w << "x" << p.first.h << "]";
            rval << " [" << p.first.tw << "x" << p.first.th << "]";
            rval << " [" << p.first.cx << "x" << p.first.cy << "]";
        rval << "\n";
        rval << "\t\t\tUsers:    " << p.second.users << "\n";
        rval << "\t\t\tMeshes:\n";
        for (auto& pm : p.second.meshes)
        {
            rval << "\t\t\t\t\"" << pm->id.name << "\"\n";
        }
    }

    rval << "\tWindow Title:   \"" << windowTitle << "\"";
        if (windowTitleShowFPS) rval << " [FPS]";
    rval << "\n";

    rval << "\tWindow ID:      " << hexify(window) << "\n";

    rval << "\tDefault Shader: " << hexify(banks->shader) << "\n";
    rval << "\tCustom Shader:  " << hexify(shader) << "\n";

    rval << "-- END CORE DIAGNOSIS --\n";

    return rval.str();
}

} // namespace Inugami
