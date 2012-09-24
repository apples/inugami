#include "loaders.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <fstream>
#include <string>
#include <sstream>

namespace Inugami {

bool loadImageFromFile(const std::string &fileName, std::vector<char> &target)
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

    if (ilLoadImage(fileName.c_str()))
    {
        if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
        {
            ILinfo imageInfo;
            iluGetImageInfo(&imageInfo);
            if (imageInfo.Origin == IL_ORIGIN_LOWER_LEFT)
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

bool loadObjFromFile(const std::string& fileName, Mesh& target)
{
    std::ifstream inFile(fileName.c_str());
    std::string inString, command, tmp[8];
    std::stringstream ss, ss2;

    while (std::getline(inFile, inString))
    {
        if (inString == "") continue;
        ss.clear();
        ss.str(inString);
        ss >> command;

        if (command == "#") continue;

        if (command == "v")
        {
            target.vertices.push_back(Mesh::Vertex3());
            ss >> target.vertices.back().x;
            ss >> target.vertices.back().y;
            ss >> target.vertices.back().z;
            continue;
        }

        if (command == "vn")
        {
            target.normals.push_back(Mesh::Vertex3());
            ss >> target.normals.back().x;
            ss >> target.normals.back().y;
            ss >> target.normals.back().z;
            continue;
        }

        if (command == "vt")
        {
            target.texCoords.push_back(Mesh::Vertex2());
            ss >> target.texCoords.back().u;
            ss >> target.texCoords.back().v;
            target.texCoords.back().v = 1.0-target.texCoords.back().v;
            continue;
        }

        if (command == "f")
        {
            int i=0;
            while (ss >> tmp[i]) ++i;

            if (i == 3)
            {
                target.triangles.push_back(Mesh::Triangle());
                for (int j=0; j<i; ++j)
                {
                    for (char &c : tmp[j]) if (c == '/') c = ' ';
                    ss2.clear();
                    ss2.str(tmp[j]);
                    ss2 >> target.triangles.back().vertices[j].pos;
                    ss2 >> target.triangles.back().vertices[j].uv;
                    ss2 >> target.triangles.back().vertices[j].norm;
                    --target.triangles.back().vertices[j].pos;
                    --target.triangles.back().vertices[j].uv;
                    --target.triangles.back().vertices[j].norm;
                }
            }

            continue;
        }
    }

    return true;
}

} // namespace Inugami
