#ifndef INUGAMI_RENDERER_H
#define INUGAMI_RENDERER_H

#include "mesh.h"

#include <list>
#include <vector>
#include <map>
#include <string>
#include <GL/glfw.h>

namespace Inugami {

/** @brief Controls OpenGL and manages graphical resources.
 *
 * This class will open a window, initialize OpenGL, loads resources, and
 * implement shaders.
 *
 * @todo Implement shader functionality.
 * @todo Implement common render techniques (shadows, bump maps, etc.).
 *
 */

class Renderer
{
public:

    /** @brief Item types for use with a @a PrintBuffer.
     */
    enum PrintItem
    {
        /** @brief Character string for direct display.
         * Prints the following character data directly to the screen, with the
         * exception of @c '\n', which forces a new line.
         */
        STRING,
        /** @brief Color to be applied to following items.
         * The specified color will be applied to all folowing items until
         * the printer displays.
         * The next three floating point numbers sent to the printer specify
         * the red, green, and blue components of the color, respectively.
         */
        COLOR
    };

    /** @brief Buffers data for display on screen.
     * Designed to be treated as if it were a @a std::istream, currently takes
     * only string and color data.  Use @a print() to flush the buffer to the
     * screen.
     */
    class PrintBuffer
    {
    public:
        PrintBuffer(Renderer *p);
        void print();

        static const unsigned int itemDataSizeIncrement;

        template <class T> PrintBuffer &operator<<(T &in)
        {
            //Assume it's a string
            std::stringstream ss;
            ss << in;
            return (*this) << ss;
        }

        PrintBuffer &operator<<(PrintItem in);
        PrintBuffer &operator<<(char in);
        PrintBuffer &operator<<(int in);
        PrintBuffer &operator<<(float in);
        PrintBuffer &operator<<(std::stringstream &in);
        PrintBuffer &operator<<(std::string &in);

    private:
        struct Item
        {
            PrintItem t;
            std::vector<char> data;
            unsigned int dataSize;
            unsigned int dataMax;
        };
        std::list<Item> buffer;
        Renderer *parent;
    };

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
    enum RenderMode
    {
        RM_3D,          ///< Typical 3D drawing mode.
        RM_2D,          ///< Orthogonal drawing mode.
        RM_INTERFACE    ///< Screen-space drawing mode.
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

    /** @brief Texture loading parameters.
     * Specifies texture parameters for use when laoding a texture.
     */
    struct TexParams
    {
        bool smooth;    ///< Bilinear filering.
        bool clamp;     ///< Texture coordinate clamping.
    };

    typedef GLuint* TexID;

    Renderer(const RenderParams &params);
    ~Renderer();

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
     * Loads a texture from disk. Textures are unloaded using @a dropTexture().
     * @param filename File to be loaded.
     * @param p Texture parameters.
     * @return Pointer to the loaded texture or @a nullptr if failure.
     * @sa loadTexture(const std::string &, const TexParams &)
     * @sa dropTexture()
     */
    GLuint* loadTexture(const char *fileName, const TexParams &p);

    /** @brief Loads a texture.
     * Loads a texture from disk. Textures are unloaded using @a dropTexture().
     * @param filename File to be loaded.
     * @param p Texture parameters.
     * @return Pointer to the loaded texture or @a nullptr if failure.
     * @sa loadTexture(const char *, const TexParams &)
     * @sa dropTexture()
     */
    GLuint* loadTexture(const std::string &fileName, const TexParams &p);

    /** @brief Sets the specified texture as the active texture.
     * Sets a texture to be used for future drawing.  The texture must have
     * been loaded using @a loadTexture().
     * @param tex Texture to be activated.
     * @sa loadTexture()
     */
    void setTexture(GLuint *tex);

    /** @brief Reloads all textures.
     * Reloads all textures from disk using the parameters supplied when first
     * loading them using @a loadTexture().
     * @sa loadTexture()
     */
    void reloadTextures();

    /** @brief Drops a texture.
     * Unloads a texture. Until all users of the texture call @a dropTexture(),
     * the texture will remain in memory.
     * @sa loadTexture()
     */
    void dropTexture(GLuint *target);

    /** @brief Loads a texture.
     * Loads a mesh from disk. Meshes are unloaded using @a dropMesh().
     * @param filename File to be loaded.
     * @return Pointer to the loaded mesh or @a nullptr if failure.
     * @sa loadMesh(const std::string &)
     * @sa dropMesh()
     */
    Mesh* loadMesh(const char *fileName);

    /** @brief Loads a texture.
     * Loads a mesh from disk. Meshes are unloaded using @a dropMesh().
     * @param filename File to be loaded.
     * @return Pointer to the loaded mesh or @a nullptr if failure.
     * @sa loadMesh(const char *)
     * @sa dropMesh()
     */
    Mesh* loadMesh(const std::string &fileName);

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

    /** @brief Custom PrintBuffer.
     * Instead of creating your own print buffer, it is possible to use this one
     * for trivial use, such as debug information.
     */
    PrintBuffer printer;

private:
    struct TextureRecord
    {
        GLuint id;
        int users;
        TexParams params;
    };

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

    GLuint blankTexture;
    std::map<std::string, TextureRecord> textures;
    std::map<std::string, MeshRecord> meshes;

    std::string windowTitle;
    bool windowTitleShowFPS;

    GLuint fontLists;
};

} // namespace Inugami

#endif // INUGAMI_RENDERER_H
