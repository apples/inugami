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

#include "camera.h"
#include "interface.h"
#include "mesh.h"
#include "opengl.h"
#include "shader.h"
#include "spritesheet.h"

#include <list>
#include <map>
#include <utility>
#include <string>
#include <vector>

namespace Inugami {

class Core
{
    typedef ::glm::mat4 Mat4;
public:
    /** @brief Parameters for screen initialization.
     */
    struct RenderParams
    {
        RenderParams();
        int width;          ///< Width of the screen in pixels.
        int height;         ///< Height of the screen in pixels.
        bool fullscreen;    ///< Takes fullscreen contol of device.
        bool vsync;         ///< Waits for vertical sync.
        int fsaaSamples;    ///< Number of samples to use for FSAA.
    };

    /** @brief Rendering modes.
     * Specifies the base viewing matrices for drawing.
     */

    enum class RenderMode
    {
        PERSPECTIVE,          ///< Typical 3D drawing mode.
        ORTHOGONAL,          ///< Orthogonal drawing mode.
        INTERFACE    ///< Screen-space drawing mode.
    };

    /** @brief Specified a face of a polygon.
     * Typically used to cull faces that are facing away from the camera.
     * Assume all faces are draw counterclockwise.
     */
    enum RenderFace
    {
        RF_FRONT,       ///< Front face.
        RF_BACK,        ///< Back face.
        RF_BOTH         ///< Both front and back face.
    };

    Core(const RenderParams &params);
    ~Core();

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
    double getInstantFrameRate();

    /** @brief Returns the current average display framerate.
     */
    double getAverageFrameRate();

    /** @brief Sets the render mode.
     * Specifies the render mode to use and which faces to @b not cull when
     * drawing.
     * @param m Rendering mode.
     * @param s Face to be shown.
     * @return @a True if success, @false if fail.
     */
    void applyCam(const Camera& in);

    void modelMatrix(const Mat4& in);

    /** @brief Loads a texture.
     * Loads a mesh from disk. Meshes are unloaded using @a dropMesh().
     * @param filename File to be loaded.
     * @return Pointer to the loaded mesh or @a nullptr if failure.
     * @sa loadMesh(const std::string &)
     * @sa dropMesh()
     */
    Mesh* loadMesh(const char *filename);

    /** @brief Loads a texture.
     * Loads a mesh from disk. Meshes are unloaded using @a dropMesh().
     * @param filename File to be loaded.
     * @return Pointer to the loaded mesh or @a nullptr if failure.
     * @sa loadMesh(const char *)
     * @sa dropMesh()
     */
    Mesh* loadMesh(const std::string &filename);

    /** @brief Reloads all meshes.
     * Reloads all meshes from disk.
     * @sa loadMesh()
     */
    void reloadMeshes();

    /** @brief Drops a mesh.
     * Unloads a mesh. Until all users of the mesh call @a dropMesh(),
     * the mesh will remain in memory.
     * @sa loadMesh()
     */
    void dropMesh(Mesh *target);

    /** @brief Sets the window title.
     * Sets the window's title.  Optionally append the average framerate to the
     * title.
     * @param text Title.
     * @param showFPS Append framerate.
     */
    void setWindowTitle(const char *text, bool showFPS);

    /** @brief Gets the current render paramters.
     * Gets a constant reference to the current render parameters.
     * @return Contant reference to render paramters.
     */
    const RenderParams& getParams();

    /** @brief Adds or updates a callback.
     * @param func Function to add or update.
     * @param freq Call frequency, in Hertz.
     * @return @a True if success, @a false if fail.
     */
    bool addCallback(void (*func)(), double freq);

    /** @brief Removes a callback.
     * @param func Function to remove.
     */
    void dropCallback(void (*func)());

    /** @brief Starts the scheduler.
     * This functions runs a loop that calls registered functions at the
     * specified intervals.  The loop continues while @a running is true.
     */
    int go();

    /** @brief Gets the interface of this window.
     * @return Interface pointer for this window.
     */
    Interface* getInterface();

    /** @brief Gets a pointer to the current shader.
     * @return Pointer to the current shader.
     */
    const Shader* getShader();

    /** @brief Sets the current shader.
     *  Sets the target shader for use in rendering. If nullptr, default shader is used.
     *  @param in Pointer to the target shader.
     */
    void setShader(Shader* in);

    int getWindowParam(int param);

    /**
     * Set this to @a false at any time to exit the @a go() cycle.
     */
    bool running;

private:
    struct Callback
    {
        void (*func)();
        double freq, wait, last;
    };

    static constexpr int MAXCALLBACKS = 32;
    Callback callbacks[MAXCALLBACKS];
    int callbackCount;

    struct MeshRecord
    {
        int users;
        Mesh mesh;
    };

    static void init();
    static void die();

    void createDefaultShader();

    double frameStartTime;
    double currentFrameRate;
    double averageFrameRate;
    double targetFrameRate;
    double invTargetFramerate;
    std::list<double> frameRateStack;
    std::list<double>::iterator frStackIterator;

    RenderParams rparams;
    float aspectRatio;

    std::map<std::string, MeshRecord> meshes;

    std::string windowTitle;
    bool windowTitleShowFPS;

    GLFWwindow window;
    Interface *iface;

    Shader *defaultShader, *customShader;
};

} // namespace Inugami

#endif // INUGAMI_CORE_H
