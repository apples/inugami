#ifndef INUGAMI_SPRITESHEET_H
#define INUGAMI_SPRITESHEET_H

#include "mesh.h"
#include "texture.h"

#include <map>
#include <vector>

namespace Inugami
{

class Spritesheet
{
public:
    Spritesheet(const std::string &filename, unsigned int w, unsigned int h, float cx = 0.5f, float cy = 0.5f);
    virtual ~Spritesheet();

    void draw(unsigned int r, unsigned int c);

    Mesh &getMesh(unsigned int r, unsigned int c);
    Texture &getTex();

private:
    struct Dimensions
    {
        bool operator<(const Dimensions &in) const;
        unsigned int w, h;
        unsigned int tw, th;
        float cx, cy;
    };

    void init(const std::string &filename, unsigned int w, unsigned int h);

    Texture tex;
    std::vector<Mesh> *sprites;

    Dimensions dim;

    static std::map<Dimensions, std::vector<Mesh>> pool;
};

} // namespace Inugami

#endif // INUGAMI_SPRITESHEET_H
