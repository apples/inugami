/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.2.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "core.hpp"

#include "camera.hpp"
#include "exception.hpp"
#include "interface.hpp"
#include "loaders.hpp"
#include "shaderprogram.hpp"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace Inugami {

int Core::numCores = 0;

class CoreException : public Exception
{
public:
    CoreException() = delete;

    CoreException(Core* c, std::string error)
        : core(c)
        , err("")
    {
        err += "Core Exception: ";
        err += error;
    }

    const char* what() const noexcept override
    {
        return err.c_str();
    }

    Core* core;
    std::string err;
};

Core::RenderParams::RenderParams()
    : width(800)
    , height(600)
    , fullscreen(false)
    , vsync(false)
    , fsaaSamples(0)
{}

Core::Core(const RenderParams &params)
    : running(false)
    , iface(nullptr)

    , callbacks()

    , frameStartTime(0.f)
    , frameRateStack(10, 0.0)
    , frStackIterator(frameRateStack.begin())

    , rparams(params)
    , aspectRatio(double(rparams.width)/double(rparams.height))

    , windowTitle("Inugami")
    , windowTitleShowFPS(false)
    , window(nullptr)

    , viewProjection(1.f)

    , shader()
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

    glClearColor(0.3, 0.3, 0.3, 0.0);

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

    shader = Shader(ShaderProgram::fromDefault());

    iface = std::unique_ptr<Interface>(new Interface(window));

    ++numCores;
}

Core::~Core()
{
    activate();

    glfwDestroyWindow(window);

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
    glfwMakeContextCurrent(nullptr);
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

    getShader().bind();

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

    try { getShader().setUniform("projectionMatrix", in.getProjection()); }
    catch (ShaderException& e) {}
    try { getShader().setUniform("viewMatrix", in.getView()); }
    catch (ShaderException& e) {}
    try { getShader().setUniform("modelMatrix", ::glm::mat4(1.f)); }
    catch (ShaderException& e) {}

    viewProjection = in.getProjection()*in.getView();
}

void Core::modelMatrix(const Mat4& in)
{
    activate();

    try { getShader().setUniform("modelMatrix", in); }
    catch (ShaderException& e) {}

    getShader().setUniform("MVP", viewProjection*in);
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

void Core::go()
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
}

const Shader& Core::getShader() const
{
    return shader;
}

void Core::setShader(const Shader& in)
{
    shader = in;
    shader.bind();
}

int Core::getWindowAttrib(int param) const
{
    return glfwGetWindowAttrib(window, param);
}

bool Core::shouldClose() const
{
    return glfwWindowShouldClose(window);
}

} // namespace Inugami
