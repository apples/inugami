/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
 * Version: 0.2.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "animatedsprite.hpp"

#include "core.hpp"
#include "math.hpp"
#include "spritesheet.hpp"
#include "utility.hpp"

#include <stdexcept>
#include <utility>

using namespace std;

namespace Inugami {

AnimatedSprite::AnimatedSprite(const Spritesheet& in)
    : flipX(false)
    , flipY(false)
    , rot(0.f)

    , sheet(in)
    , sprites()
    , sequence()
    , mode(Mode::NORMAL)

    , ended(false)
    , timer(0)
    , pos(0)
    , dir(1)
{}

AnimatedSprite::AnimatedSprite(const AnimatedSprite& in)
    : flipX(in.flipX)
    , flipY(in.flipY)
    , rot(in.rot)

    , sheet(in.sheet)
    , sprites(in.sprites)
    , sequence(in.sequence)
    , mode(in.mode)

    , ended(in.ended)
    , timer(in.timer)
    , pos(in.pos)
    , dir(in.dir)
{}

AnimatedSprite::AnimatedSprite(AnimatedSprite&& in)
    : flipX(in.flipX)
    , flipY(in.flipY)
    , rot(in.rot)

    , sheet(move(in.sheet))
    , sprites(move(in.sprites))
    , sequence(move(in.sequence))
    , mode(in.mode)

    , ended(in.ended)
    , timer(in.timer)
    , pos(in.pos)
    , dir(in.dir)
{}

AnimatedSprite::~AnimatedSprite()
{}

void AnimatedSprite::setSpritesheet(const Spritesheet& in)
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
    sprites.clear();
    sprites = in;

    sequence.clear();
}

void AnimatedSprite::setSequence(const FrameList &in)
{
    sequence.clear();
    sequence = in;
    timer = sequence[0].second;
}

void AnimatedSprite::draw(Core& core, Transform in)
{
    if (sequence.empty()) throw std::logic_error("Animation is empty!");

    if (!ended)
    {
        auto& sprite = sprites[sequence[pos].first];
        in.scale(Vec3{(flipX)?-1.f:1.f, (flipY)?-1.f:1.f, 1.f});
        in.rotate(rot, Vec3{0.f, 0.f, 1.f});
        core.modelMatrix(in);
        sheet.draw(sprite.first, sprite.second);

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
