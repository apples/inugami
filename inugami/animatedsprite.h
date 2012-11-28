#ifndef INUGAMI_ANIMATEDSPRITE_H
#define INUGAMI_ANIMATEDSPRITE_H

#include "spritesheet.h"

#include <initializer_list>
#include <utility>
#include <vector>

namespace Inugami {

class AnimatedSprite
{
public:
    enum class Mode
    {
        NORMAL,
        LOOP,
        BOUNCE
    };

    AnimatedSprite();
    virtual ~AnimatedSprite();

    void setSpritesheet(Spritesheet *in);
    void setMode(Mode in);
    void setSprites(const std::initializer_list<std::pair<unsigned int, unsigned int>> &in);
    void setSequence(const std::initializer_list<std::pair<int, unsigned int>> &in);

    void draw();
    bool done();
    void reset();

private:
    Spritesheet *sheet;
    std::vector<Mesh*> meshes;
    std::vector<std::pair<int, unsigned int>> sequence;
    Mode mode;
    bool ended;
    unsigned int timer;
    int pos, dir;
};

} // namespace Inugami

#endif // INUGAMI_ANIMATEDSPRITE_H
