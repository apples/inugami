#include "geometry.hpp"

#include "utility.hpp"

#include <fstream>
#include <sstream>
#include <utility>

using namespace std;

namespace Inugami {

Geometry::Vertex::Vertex() :
    pos(0.f, 0.f, 0.f),
    norm(0.f, 0.f, 0.f),
    tex(0.f, 0.f)
{}

bool Geometry::Vertex::operator==(const Vertex& in) const
{
    return (
        pos == in.pos
        && norm == in.norm
        && tex == in.tex
    );
}

Geometry Geometry::fromRect(float w, float h) //static
{
    Geometry geo;
    Geometry::Triangle tri;
    Geometry::Vertex vert;

    vert.norm = Vec3{-0.f, 0.f, 1.f};

    vert.pos = Vec3{-w/2.f, -h/2.f, 0.f};
    vert.tex = Vec2{0.f, 1.f};
    tri[0] = addOnce(geo.vertices, vert);

    vert.pos = Vec3{-w/2.f, h/2.f, 0.f};
    vert.tex = Vec2{0.f, 0.f};
    tri[1] = addOnce(geo.vertices, vert);

    vert.pos = Vec3{w/2.f, h/2.f, 0.f};
    vert.tex = Vec2{1.f, 0.f};
    tri[2] = addOnce(geo.vertices, vert);

    geo.triangles.push_back(tri);

    vert.pos = Vec3{w/2.f, -h/2.f, 0.f};
    vert.tex = Vec2{1.f, 1.f};
    tri[1] = addOnce(geo.vertices, vert);

    geo.triangles.push_back(tri);

    return geo;
}

Geometry Geometry::fromCube(float w, float h, float d) //static
{

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
    vector<array<VDATA,3>> tris;

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
                    tri[i] = addOnce(rval.vertices, vert);
                }

                rval.triangles.push_back(tri);
            }

            continue;
        }
    }

    return rval;
}

Geometry::Geometry()
{}

Geometry::Geometry(const Geometry& in) :
    vertices(in.vertices),
    points(in.points),
    lines(in.lines),
    triangles(in.triangles)
{}

Geometry::Geometry(Geometry&& in) :
    vertices(move(in.vertices)),
    points(move(in.points)),
    lines(move(in.lines)),
    triangles(move(in.triangles))
{}

Geometry::~Geometry()
{}

} // namespace Inugami
