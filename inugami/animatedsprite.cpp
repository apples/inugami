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

#include <stdexcept>

namespace Inugami {

AnimatedSprite::AnimatedSprite() :
    sheet(nullptr), meshes(0), sequence(0),
    mode(Mode::NORMAL), ended(false),
    timer(0),
    pos(0), dir(1),
    flip({false, false})
{}

AnimatedSprite::~AnimatedSprite()
{}

void AnimatedSprite::setSpritesheet(Spritesheet* in)
{
    sheet = in;
    meshes.clear();
    sequence.clear();
}

void AnimatedSprite::setMode(Mode in)
{
    mode = in;
}

void AnimatedSprite::setSprites(const std::vector<std::pair<unsigned int, unsigned int>> &in)
{
    if (sheet == nullptr) throw std::logic_error("Spritesheet not set!");

    meshes.clear();
    meshes.reserve(in.size());
    for (auto &p : in)
    {
        meshes.push_back(&sheet->getMesh(p.first, p.second));
    }

    sequence.clear();
}

void AnimatedSprite::setSequence(const FrameList &in)
{
    sequence.clear();
    sequence.reserve(in.size());
    for (auto &p : in)
    {
        if (p.first >= meshes.size()) throw std::out_of_range("Sprite not found!");
        if (p.second == 0) throw std::out_of_range("Duration cannot be 0.");
        sequence.push_back(p);
    }
    timer = sequence[0].second;
}

void AnimatedSprite::flipX()
{
    flip[0] = !flip[0];
}

void AnimatedSprite::flipX(bool in)
{
    flip[0] = in;
}

void AnimatedSprite::flipY()
{
    flip[1] = !flip[1];
}

void AnimatedSprite::flipY(bool in)
{
    flip[1] = in;
}

void AnimatedSprite::draw()
{
    if (sequence.empty()) throw std::logic_error("Animation is empty!");

    if (!ended)
    {
        sheet->getTex().bind();

        glScalef((flip[0])?-1.0f:1.0f, (flip[1])?-1.0f:1.0f, 1.0f);
        meshes[sequence[pos].first]->draw();

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
    flip[0] = false;
    flip[1] = false;
}

} // namespace Inugami
