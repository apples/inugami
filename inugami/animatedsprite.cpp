#include "animatedsprite.h"

#include "math.h"

#include <stdexcept>

namespace Inugami {

AnimatedSprite::AnimatedSprite() :
    sheet(nullptr), meshes(0), sequence(0),
    mode(Mode::NORMAL), ended(false),
    timer(0),
    pos(0), dir(1)
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

void AnimatedSprite::setSequence(const std::vector<std::pair<int, unsigned int>> &in)
{
    sequence.clear();
    sequence.reserve(in.size());
    for (auto &p : in)
    {
        if (std::abs(p.first) >= meshes.size()) throw std::out_of_range("Sprite not found!");
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
        sheet->getTex().bind();

        if (sequence[pos].first < 0)
        {
            glScalef(-1.0f, 1.0f, 1.0f);
            meshes[sequence[-pos].first]->draw();
        }
        else meshes[sequence[pos].first]->draw();

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
