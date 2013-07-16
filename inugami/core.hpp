/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
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

#ifndef INUGAMI_CORE_H
#define INUGAMI_CORE_H

#include "inugami.hpp"

#include "shader.hpp"
#include "transform.hpp"
#include "utility.hpp"

#include <functional>
#include <list>
#include <string>
#include <vector>

namespace Inugami {

/*! @brief %Inugami's core.
 *
 *  Oh man, the core. If you want to use %Inugami to it's full potential, you
 *  need one of these. It's recommended to derive your own core from it, to
 *  make things a bit easier.
 */
class Core
{
    friend class Interface;
public:
    /*! @brief Parameters for screen initialization.
     */
    class RenderParams
    {
    public:
        RenderParams();     //!< Default constructor.
        int width;          //!< Width of the screen in pixels.
        int height;         //!< Height of the screen in pixels.
        bool fullscreen;    //!< Fullscreen mode.
        bool vsync;         //!< Waits for vertical sync.
        int fsaaSamples;    //!< Number of samples to use for FSAA.
    };

    Core() = delete;

    /*! @brief Primary constructor.
     *
     *  Constructs a core and opens a window using the given parameters.
     *
     *  @param params A completed @ref RenderParams.
     */
    Core(const RenderParams &params);

    Core(const Core& in) = delete; //TODO Resource sharing between cores
    Core(Core&& in) = delete;

    /*! @brief Destructor.
     */
    virtual ~Core();

    Core& operator=(const Core&) = delete;
    Core& operator=(Core&&) = delete;

    /*! @brief Activates the core's context.
     */
    void activate() const;

    /*! @brief Deactivates the core's context.
     *
     *  I can't think of a situation where this would be needed, but maybe
     *  somebody else will find one.
     */
    void deactivate() const;

    /*! @brief Prepares the renderer to draw a frame.
     *
     *  Typically called at the start of a global drawing routine. This will
     *  calculate framerate, clear the screen, and reset any transformations.
     */
    void beginFrame();

    /*! @brief Finalizes drawing of a frame.
     *
     *  Typically called at the end of a global drawing routine. This will
     *  flush all drawing to the screen.
     */
    void endFrame();

    /*! @brief Returns the current graphical framerate.
     */
    double getInstantFrameRate() const;

    /*! @brief Returns the current average graphical framerate.
     */
    double getAverageFrameRate() const;

    /*! @brief Applies a camera to the scene.
     *
     *  The given camera's matrices are uploaded to the gpu.
     *
     *  @param in The camera to apply.
     */
    virtual void applyCam(const Camera& in);

    /*! @brief Applies a model matrix to the scene.
     *
     *  The given model matrix is uploaded to the gpu.
     *
     *  @param in The model matrix to apply.
     */
    virtual void modelMatrix(const Mat4& in);

    /*! @brief Sets the window title.
     *
     *  Sets the window's title. Optionally append the average framerate to the
     *  title.
     *
     *  @param text Title.
     *  @param showFPS Append framerate.
     */
    void setWindowTitle(const char *text, bool showFPS = false);

    /*! @brief Gets the current render paramters.
     *
     *  These parameters are the currently active parameters.
     *
     *  @return Current render paramters.
     */
    const RenderParams& getParams() const;

    /*! @brief Adds a callback.
     *
     *  Sets the given function to be called during the @ref go() cycle at the
     *  given frequency.
     *
     *  @param func Function to add.
     *  @param freq Call frequency, in Hertz.
     */
    void addCallback(std::function<void()> func, double freq);

    /*! @brief Removes all callbacks.
     */
    void clearCallbacks();

    /*! @brief Starts the scheduler.
     *
     *  This functions runs a loop that calls registered functions at the
     *  specified frequencies. The loop continues while Core::running is true.
     */
    void go();

    /*! @brief Gets the current shader.
     *
     *  @return The current shader.
     */
    const Shader& getShader() const;

    /*! @brief Sets the current shader.
     *
     *  Sets the given shader for use in rendering.
     *
     *  @note The given shader will be copied.
     *
     *  @param in Shader to use in rendering.
     */
    void setShader(const Shader& in);

    /*! @brief Gets a window attribute.
     *
     *  Gets a window attribute from GLFW.
     *
     *  @param param The attribute to get from GLFW.
     *
     *  @return The attribute's value.
     *
     *  @todo Disguise GLFW.
     */
    int getWindowAttrib(int param) const;

    /*! @brief Tells if the window should close.
     *
     *  Returns true if the user or OS has requested that the window be closed.
     *
     *  @return True if the window should close.
     */
    bool shouldClose() const;

    /*! @brief True if the core is running.
     *
     *  Set this to @a false at any time to exit the go() cycle.
     */
    bool running;

    /*! @brief The core's human Interface.
     */
    ConstAttr<std::unique_ptr<Interface>, Core> iface;

private:
    using Window = GLFWwindow*;

    struct Callback
    {
        std::function<void()> func;
        double freq, last;
    };

    static void init();
    static void die();

    static int numCores;

    std::vector<Callback> callbacks;

    double frameStartTime;
    std::list<double> frameRateStack;
    std::list<double>::iterator frStackIterator;

    RenderParams rparams;
    float aspectRatio;

    std::string windowTitle;
    bool windowTitleShowFPS;

    Window window;

    Mat4 viewProjection;

    Shader shader;
};

} // namespace Inugami

#endif // INUGAMI_CORE_H
