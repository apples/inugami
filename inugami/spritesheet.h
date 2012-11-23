#ifndef INUGAMI_SPRITESHEET_H
#define INUGAMI_SPRITESHEET_H

#include "renderer.h"

namespace Inugami
{

class Spritesheet
{
public:
    Spritesheet(Renderer *r, const char *fileName, unsigned int w, unsigned int h);
    Spritesheet(Renderer *r, const std::string &fileName, unsigned int w, unsigned int h);
    virtual ~Spritesheet();

    Mesh &getMesh(unsigned int r, unsigned int c);
    Renderer::Texture &getTex();

private:
    void init(Renderer *r, const std::string &fileName, unsigned int w, unsigned int h);

    Renderer *renderer;

    Renderer::Texture tex;
    std::vector<Mesh> sprites;

    unsigned int tilesW, tilesH;
};

} // namespace Inugami

#endif // INUGAMI_SPRITESHEET_H
