/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "animatedsprite.h"
#include "math.h"
#include "utility.h"

#include <stdexcept>

namespace Inugami {

AnimatedSprite::AnimatedSprite() :
    sheet(nullptr), sprites(), sequence(),
    mode(Mode::NORMAL), ended(false),
    timer(0),
    pos(0), dir(1)
{}

AnimatedSprite::AnimatedSprite(const AnimatedSprite& in) :
    sheet(new Spritesheet(*in.sheet)), sprites(in.sprites), sequence(in.sequence),
    mode(in.mode), ended(in.ended),
    timer(in.timer),
    pos(in.pos), dir(in.dir)
{}

AnimatedSprite::~AnimatedSprite()
{}

void AnimatedSprite::setSpritesheet(Spritesheet* in)
{
    sheet = in;
    sprites.clear();
    sequence.clear();
}

void AnimatedSprite::setMode(Mode in)
{
    mode = in;
}

void AnimatedSprite::setSprites(const SpriteList &in)
{
    if (sheet == nullptr) throw std::logic_error("Spritesheet not set!");

    sprites.clear();
    sprites = in;

    sequence.clear();
}

void AnimatedSprite::setSequence(const FrameList &in)
{
    sequence.clear();
    sequence.reserve(in.size());
    for (auto &p : in)
    {
        if (p.first >= sprites.size()) throw std::out_of_range("Sprite not found!");
        if (p.second == 0) throw std::out_of_range("Duration cannot be 0.");
        sequence.push_back(p);
    }
    timer = sequence[0].second;
}

void AnimatedSprite::draw()
{
    if (sequence.empty()) throw std::logic_error("Animation is empty!");

    if (!ended)
    {
        auto& sprite = sprites[sequence[pos].first];
        sheet->draw(sprite.first, sprite.second);

        if (--timer == 0)
        {
            pos += dir;
            if (pos < 0 || pos >= int(sequence.size()))
            {
                switch (mode)
                {
                case Mode::NORMAL:
                    ended = true;
                    break;
                case Mode::LOOP:
                    pos = 0;
                    break;
                case Mode::BOUNCE:
                    dir = -dir;
                    if (dir == 1) pos = 1;
                    else pos = sequence.size()-2;
                    break;
                default:
                    throw std::logic_error("Unknown mode!");
                }
            }
            if (!ended) timer = sequence[pos].second;
        }
    }
}

bool AnimatedSprite::done()
{
    return ended;
}

void AnimatedSprite::reset()
{
    pos = 0;
    dir = 1;
    ended = false;
    if (sequence.size() != 0) timer = sequence[pos].second;
}

} // namespace Inugami
