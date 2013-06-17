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

#ifndef INUGAMI_GEOMETRY_H
#define INUGAMI_GEOMETRY_H

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>

namespace Inugami {

/*! @brief Container for polygonal data.
 *
 *  Describes the structure of a 3D model using vertices grouped by points,
 *  lines, and triangles. Designed to be converted into a Mesh.
 */
class Geometry
{
public:
    using Vec3 = ::glm::vec3;
    using Vec2 = ::glm::vec2;

    /*! @brief Vertex.
     *
     *  Contains all information about a vertex.
     */
    class Vertex
    {
    public:
        Vertex();                                   //!< Default constructor.
        bool operator==(const Vertex& in) const;    //!< Equality operator.
        Vec3 pos;                                   //!< Position.
        Vec3 norm;                                  //!< Normal.
        Vec2 tex;                                   //!< Texture coordinate.
    };

    /*! @brief A single point.
     */
    using Point    = std::array<int,1>;

    /*! @brief A line segment connecting two vertices.
     */
    using Line     = std::array<int,2>;

    /*! @brief A triangle connecting three vertices.
     */
    using Triangle = std::array<int,3>;

    /*! @brief Create a Geometry from a rectangle.
     *
     *  Creates a Geometry made of 4 vertices that form a rectangle with the
     *  given dimensions.
     *
     *  @param w Width (X axis).
     *  @param h Height (Y axis).
     *
     *  @return Geometry containing a rectangle.
     */
    static Geometry fromRect(float w, float h, float cx=0.5f, float cy=0.5f);

    //TODO static Geometry::fromCuboid(float w, float h, float d);

    /*! @brief Create a Geometry from an OBJ file.
     *
     *  Creates a Geometry that is equivalent to the geometry defined in the
     *  given OBJ file.
     *
     *  @note Supported OBJ definitions are "v", "vn", "vt", and "f".
     *
     *  @param filename Name of the OBJ file to import.
     *
     *  @return Geometry imported from the OBJ file.
     */
    static Geometry fromOBJ(const std::string& filename);

    /*! @brief Default constructor.
     */
    Geometry();

    /*! @brief Copy constructor.
     */
    Geometry(const Geometry& in);

    /*! @brief Move constructor.
     */
    Geometry(Geometry&& in);

    /*! @brief Destructor.
     */
    virtual ~Geometry();

    /*! @brief Copy assignment operator.
     */
    Geometry& operator=(const Geometry& in);

    /*! @brief Move assignment operator.
     */
    Geometry& operator=(Geometry&& in);

    /*! @brief Combination operator.
     */
    Geometry& operator+=(const Geometry& in);

    std::vector<Vertex>   vertices;

    std::vector<Point>    points;
    std::vector<Line>     lines;
    std::vector<Triangle> triangles;
};

} // namespace Inugami

#endif // INUGAMI_GEOMETRY_H
