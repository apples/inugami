#ifndef INUGAMI_ANIMATEDSPRITE_H
#define INUGAMI_ANIMATEDSPRITE_H

#include "spritesheet.h"

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

    typedef std::pair<unsigned int, unsigned int> Sprite;
    typedef std::pair<unsigned int, unsigned int> Frame;
    typedef std::vector<Sprite> SpriteList;
    typedef std::vector<Frame> FrameList;

    AnimatedSprite();
    virtual ~AnimatedSprite();

    void setSpritesheet(Spritesheet *in);
    void setMode(Mode in);
    void setSprites(const SpriteList &in);
    void setSequence(const FrameList &in);

    void flipX();
    void flipX(bool in);
    void flipY();
    void flipY(bool in);

    void draw();
    bool done();
    void reset();

private:
    Spritesheet *sheet;
    std::vector<Mesh*> meshes;
    FrameList sequence;
    Mode mode;
    bool ended;
    unsigned int timer;
    int pos, dir;
    bool flip[2];
};

} // namespace Inugami

#endif // INUGAMI_ANIMATEDSPRITE_H
