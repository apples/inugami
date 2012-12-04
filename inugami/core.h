#ifndef INUGAMI_CORE_H
#define INUGAMI_CORE_H

#include "mesh.h"
#include "camera.h"
#include "opengl.h"
#include "spritesheet.h"

#include <list>
#include <map>
#include <utility>
#include <string>
#include <vector>

namespace Inugami {

class Core
{
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
        double fov;         ///< Field of view in degrees.
        float nearClip;     ///< Near clipping plane distance.
        float farClip;      ///< Far clipping plane distance.
        int fsaaSamples;    ///< Number of samples to use for FSAA.
        struct Mode2D_t
        {
            Mode2D_t();
            float width;        ///< Width of 2D space.
            float height;       ///< Height of 2D space.
            float nearClip;     ///< Near clipping plane distance.
            float farClip;      ///< Far clipping plane distance.
        } mode2D;           ///< Parameters for 2D mode.
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

    /** @brief Draws text on the screen.
     * Draws text to OpenGL, even in 3D mode, using the current transformation
     * and viewing matrices.
     * @param text String to draw.
     * @param pushpop Restore matrices when done.
     */
    void drawString(const char *text, bool pushpop=true);

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
    bool setMode(RenderMode m, RenderFace s);

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

    /** @brief Dumps state to a stream.
     * Dumps readable information about everything to the specified stream.
     * @param out Output stream.
     */
    void dumpState(std::ostream &&out);

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

    /**
     * Set this to @a false at any time to exit the @a go() cycle.
     */
    bool running;

    Camera cam;

private:
    struct Callback
    {
        void (*func)();
        double freq, wait, last;
    };

    static const int MAXCALLBACKS = 32;
    Callback callbacks[MAXCALLBACKS];
    int callbackCount;

    struct MeshRecord
    {
        int users;
        Mesh mesh;
    };

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

    GLuint fontLists;
};

} // namespace Inugami

#endif // INUGAMI_CORE_H
