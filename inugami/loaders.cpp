/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "loaders.h"

#include "math.h"
#include "utility.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <array>
#include <fstream>
#include <string>
#include <sstream>

namespace Inugami {

bool loadImageFromFile(const std::string &filename, std::vector<char> &target)
{
    static bool initialized = false;
    if (!initialized)
    {
        ilInit();
        initialized = true;
    }

    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoadImage(filename.c_str()))
    {
        if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
        {
            ILinfo imageInfo;
            iluGetImageInfo(&imageInfo);
            if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
            {
                iluFlipImage();
            }

            target.resize(imageInfo.SizeOfData+sizeof(int)*2);
            *reinterpret_cast<int*>(&target[0]) = imageInfo.Width;
            *reinterpret_cast<int*>(&target[sizeof(int)]) = imageInfo.Height;

            char *data = reinterpret_cast<char*>(ilGetData());

            for (unsigned int i=0; i<imageInfo.SizeOfData; ++i)
            {
                target[i+8] = data[i];
            }

            ilDeleteImages(1, &imageID);

            return true;
        }
    }

    ilDeleteImages(1, &imageID);
    return false;
}

bool loadTextFromFile(const std::string &filename, std::string &target)
{
    std::ifstream inFile(filename.c_str(), std::ios::ate);
    if (!inFile) return false;
    unsigned int length = inFile.tellg();
    target.resize(length);
    inFile.seekg(0);
    std::getline(inFile, target, '\0');
    return true;
}

bool loadObjFromFile(const std::string& filename, Mesh& target)
{
    typedef ::glm::vec3 vec3;
    typedef ::glm::vec2 vec2;

    std::ifstream inFile(filename.c_str());
    std::string inString, command, pointstr[8], elem[3];
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

        if (command == "#") continue;

        if (command == "v")
        {
            positions.push_back(vec3());
            ss >> positions.back().x;
            ss >> positions.back().y;
            ss >> positions.back().z;
            continue;
        }

        if (command == "vn")
        {
            normals.push_back(vec3());
            ss >> normals.back().x;
            ss >> normals.back().y;
            ss >> normals.back().z;
            continue;
        }

        if (command == "vt")
        {
            texcoords.push_back(vec2());
            ss >> texcoords.back().x;
            ss >> texcoords.back().y;
            continue;
        }

        if (command == "f")
        {
            int np=0;
            while (ss >> pointstr[np]) ++np;

            if (np == 3)
            {
                tris.push_back(std::array<VDATA, 3>());
                for (int i=0; i<np; ++i)
                {
                    size_t firstSlash = pointstr[i].find("/");
                    if (firstSlash != std::string::npos)
                    {
                        elem[0] = pointstr[i].substr(0,firstSlash);
                        pointstr[i] = pointstr[i].substr(firstSlash+1);
                        size_t secondSlash = pointstr[i].find("/");
                        if (secondSlash != std::string::npos)
                        {
                            elem[1] = pointstr[i].substr(0,secondSlash);
                            elem[2] = pointstr[i].substr(secondSlash+1);
                        }
                        else
                        {
                            elem[1] = pointstr[i];
                            elem[2] = "0";
                        }
                    }
                    else
                    {
                        elem[0] = pointstr[i];
                        elem[1] = "0";
                        elem[2] = "0";
                    }

                    ss2.clear();
                    ss2.str("");
                    ss2 << elem[0] << " " << elem[1] << " " << elem[2];
                    ss2 >> tris.back()[i].p;
                    ss2 >> tris.back()[i].t;
                    ss2 >> tris.back()[i].n;
                    --tris.back()[i].p;
                    --tris.back()[i].t;
                    --tris.back()[i].n;
                }
            }

            continue;
        }
    }

    target.reserve(high(positions.size(), normals.size(), texcoords.size()), tris.size());

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
            tri.v[i] = target.addVertex(v);
        }
        target.addTriangle(tri);
    }

    return true;
}

} // namespace Inugami
