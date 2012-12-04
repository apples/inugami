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
