/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
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

#include "geometry.hpp"

#include "../../opengl.hpp"

#include <fstream>
#include <sstream>
#include <utility>
#include <tuple>

namespace Inugami {

namespace Resources_detail {

bool Geometry::Vertex::operator==(const Vertex& in) const
{
    return (
           std::tie(   pos,    norm,    tex)
        == std::tie(in.pos, in.norm, in.tex)
    );
}

Geometry Geometry::fromRect(float w, float h, float cx, float cy) //static
{
    Geometry geo;
    Geometry::Triangle tri;
    Geometry::Vertex vert;

    vert.norm = glm::vec3{-0.f, 0.f, 1.f};

    vert.pos = glm::vec3{-w+w*cx, -h+h*cy, 0.f};
    vert.tex = glm::vec2{0.f, 0.f};
    geo.vertices.push_back(vert);
    tri[0] = 0;

    vert.pos = glm::vec3{-w+w*cx, h*cy, 0.f};
    vert.tex = glm::vec2{0.f, 1.f};
    geo.vertices.push_back(vert);
    tri[1] = 1;

    vert.pos = glm::vec3{w*cx, h*cy, 0.f};
    vert.tex = glm::vec2{1.f, 1.f};
    geo.vertices.push_back(vert);
    tri[2] = 2;

    geo.triangles.push_back(tri);

    vert.pos = glm::vec3{w*cx, -h+h*cy, 0.f};
    vert.tex = glm::vec2{1.f, 0.f};
    geo.vertices.push_back(vert);
    tri[1] = 3;

    geo.triangles.push_back(tri);

    return geo;
}

} // namespace Resources_detail

} // namespace Inugami
