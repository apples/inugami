/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
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

#ifndef INUGAMI_SPRITESHEET_H
#define INUGAMI_SPRITESHEET_H

#include "inugami.hpp"

#include "mesh.hpp"
#include "texture.hpp"

#include <vector>

namespace Inugami {

/*! @brief Managed spritesheet.
 *
 *  A Spritesheet is a collection of rectangular meshes designed for use with a
 *  specific texture.
 */
class Spritesheet
{
public:
    Spritesheet() = delete;

    /*! @brief Raw Image constructor.
     *
     *  @param img Image to use as a texture.
     *  @param tw Width of each sprite, in pixels.
     *  @param th Height of each sprite, in pixels.
     *  @param cx X-center of each sprite, range 0 to 1.
     *  @param cy Y-center of each sprite, range 0 to 1.
     */
    Spritesheet(const Image& img, int tw, int th, float cx=0.5f, float cy=0.5f);

    /*! @brief Texture constructor.
     *
     *  @param in Texture.
     *  @param tw Width of each sprite, in pixels.
     *  @param th Height of each sprite, in pixels.
     *  @param cx X-center of each sprite, range 0 to 1.
     *  @param cy Y-center of each sprite, range 0 to 1.
     */
    Spritesheet(const Texture& in, int tw, int th, float cx=0.5f, float cy=0.5f);

    /*! @brief Draws a sprite.
     *
     *  @param r Row index of sprite.
     *  @param c Column index of sprite.
     */
    void draw(int r, int c) const;

    ConstAttr<int,Spritesheet> tilesX;  //!< Number of tile columns.
    ConstAttr<int,Spritesheet> tilesY;  //!< Number of tile rows.

private:
    void generateMeshes(int tw, int th, float cx, float cy);

    Texture tex;
    std::vector<Mesh> meshes;
};

} // namespace Inugami

#endif // INUGAMI_SPRITESHEET_H
