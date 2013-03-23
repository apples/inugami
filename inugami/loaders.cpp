/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "loaders.hpp"

#include "math.hpp"
#include "mesh.hpp"
#include "utility.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <string>
#include <sstream>

namespace Inugami {

std::string loadTextFromFile(const std::string &filename)
{
    std::ifstream inFile(filename.c_str(), std::ios::binary);
    if (!inFile) throw std::runtime_error("Could not open file");

    std::string rval;
    inFile.seekg(0, std::ios::end);
    rval.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&rval[0], rval.size());

    return rval;
}

bool loadObjFromFile(const std::string& filename, Mesh::Value* target)
{
    using ::glm::vec3;
    using ::glm::vec2;

    std::ifstream inFile(filename.c_str());
    std::string inString, command, pointstr[8];
    std::stringstream ss, ss2;

    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec2> texcoords;

    struct VDATA {VDATA():p(-1),n(-1),t(-1){} int p, n, t;};
    std::vector<std::array<VDATA, 3>> tris;

    //READ FILE

    while (std::getline(inFile, inString))
    {
        if (inString == "") continue;
        ss.clear();
        ss.str(inString);
        ss >> command;

        if (command[0] == '#') continue;

        if (command == "v")
        {
            vec3 tmp;
            ss >> tmp.x;
            ss >> tmp.y;
            ss >> tmp.z;
            positions.push_back(tmp);
            continue;
        }

        if (command == "vn")
        {
            vec3 tmp;
            ss >> tmp.x;
            ss >> tmp.y;
            ss >> tmp.z;
            normals.push_back(tmp);
            continue;
        }

        if (command == "vt")
        {
            vec2 tmp;
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
                std::array<VDATA, 3> tmptri;
                for (int i=0; i<np; ++i)
                {
                    std::replace(pointstr[i].begin(), pointstr[i].end(), '/', ' ');
                    ss2.clear();
                    ss2.str(pointstr[i]);
                    if (ss2 >> tmptri[i].p)
                    {
                        --tmptri[i].p;

                        if (ss2 >> tmptri[i].t)
                        {
                            --tmptri[i].t;

                            if (ss2 >> tmptri[i].n)
                            {
                                --tmptri[i].n;
                            }
                            else
                            {
                                tmptri[i].t = -1;
                            }
                        }
                        else
                        {
                            tmptri[i].n = -1;
                            tmptri[i].t = -1;
                        }
                    }
                    else
                    {
                        tmptri[i].p = -1;
                        tmptri[i].n = -1;
                        tmptri[i].t = -1;
                    }
                }
                tris.push_back(tmptri);
            }

            continue;
        }
    }

    target->reserve(high(positions.size(), normals.size(), texcoords.size()), tris.size());

    //FORMAT DATA

    for (std::array<VDATA, 3> &vd : tris)
    {
        Mesh::Triangle tri;
        for (int i=0; i<3; ++i)
        {
            Mesh::Vertex v;
            if (positions.size()>0) v.pos = positions[vd[i].p];
            if (normals.size()>0) v.norm = normals[vd[i].n];
            if (texcoords.size()>0) v.uv = texcoords[vd[i].t];
            tri.v[i] = target->addVertex(v);
        }
        target->addTriangle(tri);
    }

    return true;
}

} // namespace Inugami
