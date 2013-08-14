/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
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

#ifndef INUGAMI_ANIMATEDSPRITE_H
#define INUGAMI_ANIMATEDSPRITE_H

#include "inugami.hpp"
#include "spritesheet.hpp"
#include "transform.hpp"

#include <array>
#include <utility>
#include <vector>

namespace Inugami {

/*! @brief A 2D animated sprite.
 *
 *  This class represents a 2D frame-based sprite animation. It is defined as
 *  a series of sprites from a single @ref Spritesheet that are each active for
 *  a certain number of frames.
 */
class AnimatedSprite
{
public:
    /*! @brief Animation mode.
     *
     *  Tells the @ref AnimatedSprite how to behave when the animation finishes.
     */
    enum class Mode
    {
          NORMAL    //! Stop at end.
        , LOOP      //! Loop back to start.
        , BOUNCE    //! Reverse back and forth.
    };

    /*! @brief Sprite coordinates
     *
     *  Sprites are a pair of integers; a row and a column.
     */
    using Sprite = std::pair<unsigned int, unsigned int>;

    /*! @brief Frame specifier
     *
     *  Frames are a pair of integers; a sprite index and a duration (ticks).
     */
    using Frame = std::pair<unsigned int, unsigned int>;

    /*! @brief List of Sprites
     */
    using SpriteList = std::vector<Sprite>;

    /*! @brief List of frames
     */
    using FrameList = std::vector<Frame>;

    AnimatedSprite() = delete;

    /*! @brief Primary constructor
     *
     *  Constructs an AnimatedSprite that uses the given @ref Spritesheet to
     *  draw. The given @ref Spritesheet will be copied.
     *
     *  @param in The @ref Spritesheet to use for drawing.
     */
    AnimatedSprite(Spritesheet in);

    /*! @brief Sets the @ref Spritesheet to use for drawing.
     *
     *  @note The given @ref Spritesheet will be copied.
     *  @note The current @ref SpriteList will be cleared.
     *  @note The current @ref FrameList will be cleared.
     *
     *  @param in The @ref Spritesheet to use for drawing.
     */
    void setSpritesheet(Spritesheet in);

    /*! @brief Sets the @ref Mode to use.
     *
     *  The given @ref Mode will be used when cycling the frames.
     *
     *  @param in The @ref Mode to use for drawing.
     */
    void setMode(Mode in);

    /*! @brief Sets the @ref SpriteList to use for drawing.
     *
     *  @note The given @ref SpriteList will be copied.
     *  @note The current @ref FrameList will be cleared.
     *
     *  @param in The @ref SpriteList to use for drawing.
     */
    void setSprites(SpriteList in);

    /*! @brief Sets the @ref FrameList to use for drawing.
     *
     *  @note The given @ref FrameList will be copied.
     *
     *  @param in The @ref Spritesheet to use for drawing.
     */
    void setSequence(FrameList in);

    /*! @brief Draws the current frame.
     *
     *  Draws the current frame using the given @ref Transform.
     *
     *  @note The core's model matrix will be set.
     *
     *  @param core The @ref Core to use for drawing.
     *  @param in The @ref Transform to use as the origin.
     */
    void draw(Core& core, Transform in) const;

    /*! @brief Advances the sprite's frame.
     *
     *  Increments the timer, advancing the frame if necessary. If the animation
     *  reaches it's end, it will reset according to the @ref Mode.
     */
    void tick();

    /*! @brief Check if the animation is done.
     *
     *  @note Will only return true when the mode is @ref Mode::NORMAL.
     */
    bool done();

    /*! @brief Resets the animation.
     *
     *  The animation will be reset to the first frame.
     */
    void reset();

    /*! @brief Flips the sprite on the X axis.
     */
    bool flipX;

    /*! @brief Flips the sprite on the Y axis.
     */
    bool flipY;

    /*! @brief Rotates the sprite by the given degrees on the Z axis.
     */
    float rot;

private:
    Spritesheet sheet;
    SpriteList sprites;
    FrameList sequence;
    Mode mode;
    bool ended;
    int timer;
    int pos, dir;
};

} // namespace Inugami

#endif // INUGAMI_ANIMATEDSPRITE_H
