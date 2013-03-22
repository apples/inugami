/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_ANIMATEDSPRITE_H
#define INUGAMI_ANIMATEDSPRITE_H

#include "inugami.hpp"
#include "transform.hpp"

#include <array>
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

    AnimatedSprite(Core& in);
    AnimatedSprite(const AnimatedSprite& in);
    virtual ~AnimatedSprite();

    void setSpritesheet(Spritesheet *in);
    void setMode(Mode in);
    void setSprites(const SpriteList &in);
    void setSequence(const FrameList &in);

    void draw(Core& core, Transform in);
    bool done();
    void reset();

    bool flipX, flipY;
    float rot;

private:
    Spritesheet *sheet;
    SpriteList sprites;
    FrameList sequence;
    Mode mode;
    bool ended;
    unsigned int timer;
    int pos, dir;
};

} // namespace Inugami

#endif // INUGAMI_ANIMATEDSPRITE_H
