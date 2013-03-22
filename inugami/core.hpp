/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_CORE_H
#define INUGAMI_CORE_H

#include "inugami.hpp"

#include "opengl.hpp"

#include "mesh.hpp"
#include "texture.hpp"
#include "spritesheet.hpp"
#include "transform.hpp"
#include "utility.hpp"

#include <functional>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Inugami {

class Core
{
    friend class Mesh;
    friend class Texture;
    friend class Spritesheet;
public:
    /** @brief Parameters for screen initialization.
     */
    class RenderParams
    {
    public:
        RenderParams();
        int width;          ///< Width of the screen in pixels.
        int height;         ///< Height of the screen in pixels.
        bool fullscreen;    ///< Takes fullscreen contol of device.
        bool vsync;         ///< Waits for vertical sync.
        int fsaaSamples;    ///< Number of samples to use for FSAA.
    };

    using Window = GLFWwindow*;

    Core() = delete;
    Core(const RenderParams &params);
    Core(const Core &in) = delete; //TODO Resource sharing between cores
    virtual ~Core();

    void activate() const;
    void deactivate() const;

    /** @brief Prepares the renderer to draw a frame.
     * Typically called at the start of a global drawing routine.
     * This function will calculate framerate, clear the screen, and reset any
     * transformations.
     */
    void beginFrame();

    /** @brief Finalizes drawing of a frame.
     * Typically called at the end of a global drawing routine.
     * This function will flush all drawing to the screen.
     */
    void endFrame();

    /** @brief Returns the current display framerate.
     */
    double getInstantFrameRate() const;

    /** @brief Returns the current average display framerate.
     */
    double getAverageFrameRate() const;

    /** @brief Sets the render mode.
     * Specifies the render mode to use and which faces to @b not cull when
     * drawing.
     * @param m Rendering mode.
     * @param s Face to be shown.
     * @return @a True if success, @false if fail.
     */
    void applyCam(const Camera& in);

    void modelMatrix(const Mat4& in);

    /** @brief Sets the window title.
     * Sets the window's title.  Optionally append the average framerate to the
     * title.
     * @param text Title.
     * @param showFPS Append framerate.
     */
    void setWindowTitle(const char *text, bool showFPS = false);

    /** @brief Gets the current render paramters.
     * Gets a constant reference to the current render parameters.
     * @return Contant reference to render paramters.
     */
    const RenderParams& getParams() const;

    /** @brief Adds or updates a callback.
     * @param func Function to add.
     * @param freq Call frequency, in Hertz.
     */
    void addCallback(std::function<void()> func, double freq);

    /** @brief Removes all callbacks.
     */
    void clearCallbacks();

    /** @brief Starts the scheduler.
     * This functions runs a loop that calls registered functions at the
     * specified intervals.  The loop continues while @a running is true.
     */
    int go();

    /** @brief Gets a pointer to the current shader.
     * @return Pointer to the current shader.
     */
    const Shader* getShader() const;

    /** @brief Sets the current shader.
     *  Sets the target shader for use in rendering. If nullptr, default shader is used.
     *  @param in Pointer to the target shader.
     */
    void setShader(Shader* in);

    int getWindowParam(int param) const;

    std::string getDiagnostic() const;

    /**
     * Set this to @a false at any time to exit the @a go() cycle.
     */
    bool running;

    ConstAttr<Interface*, Core> iface;

private:
    struct Callback
    {
        std::function<void()> func;
        double freq, last;
    };

    static void init();
    static void die();

    static int numCores;

    void createDefaultShader();

    std::vector<Callback> callbacks;

    double frameStartTime;
    std::list<double> frameRateStack;
    std::list<double>::iterator frStackIterator;

    RenderParams rparams;
    float aspectRatio;

    std::string windowTitle;
    bool windowTitleShowFPS;

    Window window;

    Shader *shader;

    SharedBank* banks;
};

} // namespace Inugami

#endif // INUGAMI_CORE_H
