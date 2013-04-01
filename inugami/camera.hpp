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

#ifndef INUGAMI_CAMERA_H
#define INUGAMI_CAMERA_H

#include "opengl.hpp"

namespace Inugami {

/*! @brief A 3D camera.
 * 
 *  This class represents a camera in 3D space. It has the familiar GLU-style
 *  viewport functions, can easily be translated and rotated, and defines
 *  how polygons are culled.
 */
class Camera
{
public:
    using Vec3 = ::glm::vec3;
    using Mat4 = ::glm::mat4;

    /*! @brief Default contructor.
     * 
     *  Constructs the camera with identity matrices.
     */
    Camera();
    
    /*! @brief Copy constructor.
     */
    Camera(const Camera&) = default;
    
    /*! @brief Destructor
     */
    virtual ~Camera();

    /*! @brief Applies a perspective viewport.
     * 
     *  Sets up the matrices for perspective projection.
     * 
     *  @param fov Vertical field of view.
     *  @param ratio Ratio of width to height.
     *  @param near Near clipping plane.
     *  @param far Far clipping plane.
     * 
     *  @return *this
     */
    Camera& perspective(float fov, float ratio, float near, float far);
    
    /*! @brief Applies an orthogonal viewport.
     * 
     *  Sets up the matrices for orthogonal projection.
     * 
     *  @param left Left edge of screen.
     *  @param right Right edge of screen.
     *  @param bottom Bottom edge of screen.
     *  @param near Near clipping plane.
     *  @param far Far clipping plane.
     * 
     *  @return *this
     */
    Camera& ortho(float left, float right, float bottom, float top, float near, float far);

    /*! @brief Moves the camera.
     * 
     *  Translates the camera to the given location, relative to the current
     *  position.
     * 
     *  @param pos Relative translation vector.
     * 
     *  @return *this
     */
    Camera& translate(const Vec3& pos);
    
    /*! @brief Rotates the camera.
     * 
     *  Rotates the camera by the given degrees about the give axis.
     * 
     *  @param deg Degrees to rotate.
     *  @param axis Axis of rotation.
     * 
     *  @return *this
     */
    Camera& rotate(float deg, const Vec3& axis);

    /*! @brief Pitches the camera.
     * 
     *  Rotates the camera along the X axis.
     * 
     *  @param deg Degrees to rotate.
     * 
     *  @return *this
     */
    Camera& pitch(float deg);
    
    /*! @brief Yaws the camera.
     * 
     *  Rotates the camera along the Y axis.
     * 
     *  @param deg Degrees to rotate.
     * 
     *  @return *this
     */
    Camera& yaw(float deg);
    
    /*! @brief Rolls the camera.
     * 
     *  Rotates the camera along the Z axis.
     * 
     *  @param deg Degrees to rotate.
     * 
     *  @return *this
     */
    Camera& roll(float deg);

    /*! @brief Gets the projection matrix.
     * 
     *  @return Projection matrix.
     */
    const Mat4& getProjection() const;
    
    /*! @brief Gets the view matrix.
     * 
     *  @return View matrix.
     */
    const Mat4& getView() const;

    /*! @brief Only draws the front of a face.
     */
    bool cullFaces;
    
    /*! @brief Applies depth testing.
     */
    bool depthTest;

private:
    Mat4 projection;
    Mat4 view;
};

} // namespace Inugami

#endif // INUGAMI_CAMERA_H
