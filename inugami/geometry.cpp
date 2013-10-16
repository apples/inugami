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

#include "geometry.hpp"

#include "math.hpp"
#include "utility.hpp"

#include <fstream>
#include <sstream>
#include <utility>
#include <tuple>

using namespace std;

namespace Inugami {

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

    vert.norm = Vec3{-0.f, 0.f, 1.f};

    vert.pos = Vec3{-w+w*cx, -h+h*cy, 0.f};
    vert.tex = Vec2{0.f, 0.f};
    geo.vertices.push_back(vert);
    tri[0] = 0;

    vert.pos = Vec3{-w+w*cx, h*cy, 0.f};
    vert.tex = Vec2{0.f, 1.f};
    geo.vertices.push_back(vert);
    tri[1] = 1;

    vert.pos = Vec3{w*cx, h*cy, 0.f};
    vert.tex = Vec2{1.f, 1.f};
    geo.vertices.push_back(vert);
    tri[2] = 2;

    geo.triangles.push_back(tri);

    vert.pos = Vec3{w*cx, -h+h*cy, 0.f};
    vert.tex = Vec2{1.f, 0.f};
    geo.vertices.push_back(vert);
    tri[1] = 3;

    geo.triangles.push_back(tri);

    return geo;
}

Geometry Geometry::fromDisc(float w, float h, int e) //static
{
    if (e<3) throw GeometryError("fromDisc: Must have at least 3 edges!");

    Geometry geo;
    Geometry::Triangle tri;
    Geometry::Vertex vert;

    const float dDeg = 360.f/float(e);

    w *= 0.5;
    h *= 0.5;

    vert.pos  = Vec3{0.f, 0.f, 0.f};
    vert.norm = Vec3{0.f, 0.f, 1.f};
    vert.tex  = Vec2{0.5f, 0.5f};

    geo.vertices.push_back(vert);
    tri[0] = 0;
    tri[1] = 0;
    tri[2] = 0;

    float deg = -dDeg;
    vert.pos = Vec3{cosd(deg)*w, sind(deg)*h, 0.f};
    vert.tex = Vec2{cosd(deg),   sind(deg)};

    tri[1] = 1;
    geo.vertices.push_back(vert);

    for (int i=0; i<e-1; ++i)
    {
        deg = dDeg*i;
        vert.pos = Vec3{cosd(deg)*w, sind(deg)*h, 0.f};
        vert.tex = Vec2{cosd(deg),   sind(deg)};

        tri[2] = geo.vertices.size();
        geo.vertices.push_back(vert);
        geo.triangles.push_back(tri);
        tri[1] = tri[2];
    }

    tri[2] = 1;
    geo.triangles.push_back(tri);

    return geo;
}

Geometry Geometry::fromOBJ(const string& filename) //static
{
    Geometry rval;

    ifstream inFile(filename.c_str());
    string inString, command, pointstr[8];
    stringstream ss, ss2;

    vector<Vec3> positions;
    vector<Vec3> normals;
    vector<Vec2> texcoords;

    struct VDATA {VDATA():p(-1),n(-1),t(-1){} int p, n, t;};

    //READ FILE

    while (getline(inFile, inString))
    {
        if (inString == "") continue;

        ss.clear();
        ss.str(inString);
        ss >> command;

        if (command[0] == '#') continue;

        if (command == "v")
        {
            Vec3 tmp;
            ss >> tmp.x;
            ss >> tmp.y;
            ss >> tmp.z;
            positions.push_back(tmp);
            continue;
        }

        if (command == "vn")
        {
            Vec3 tmp;
            ss >> tmp.x;
            ss >> tmp.y;
            ss >> tmp.z;
            normals.push_back(tmp);
            continue;
        }

        if (command == "vt")
        {
            Vec2 tmp;
            ss >> tmp.x;
            ss >> tmp.y;
            texcoords.push_back(tmp);
            continue;
        }

        if (command == "f")
        {
            int np=0;
            while (ss >> pointstr[np]) ++np;

            if (np == 3)
            {
                array<VDATA, 3> tmptri;
                Triangle tri;

                for (int i=0; i<3; ++i)
                {
                    auto pos = pointstr[i].find('/');
                    while (pos != string::npos)
                    {
                        pointstr[i][pos] = ' ';
                        pointstr[i].insert(pos+1, "0");
                        pos = pointstr[i].find('/', pos+2);
                    }

                    ss2.clear();
                    ss2.str(pointstr[i]);

                    ss2 >> tmptri[i].p >> tmptri[i].t >> tmptri[i].n;
                    --tmptri[i].p;
                    --tmptri[i].t;
                    --tmptri[i].n;

                    Vertex vert;
                    if (tmptri[i].p>=0) vert.pos  = positions[tmptri[i].p];
                    if (tmptri[i].n>=0) vert.norm =   normals[tmptri[i].n];
                    if (tmptri[i].t>=0) vert.tex  = texcoords[tmptri[i].t];
                    tri[i] = addOnceVec(rval.vertices, vert);
                }

                rval.triangles.push_back(tri);
            }

            continue;
        }
    }

    return rval;
}

Geometry& Geometry::operator+=(const Geometry& in)
{
    vertices .insert(end( vertices), begin(in. vertices), end(in. vertices));
    points   .insert(end(   points), begin(in.   points), end(in.   points));
    lines    .insert(end(    lines), begin(in.    lines), end(in.    lines));
    triangles.insert(end(triangles), begin(in.triangles), end(in.triangles));
    return *this;
}

GeometryError::GeometryError(std::string in)
    : err("Geometry Error: "+in)
{}

const char* GeometryError::what() const noexcept
{
    return err.c_str();
}

} // namespace Inugami
