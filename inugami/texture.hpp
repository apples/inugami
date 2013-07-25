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

#ifndef INUGAMI_TEXTURE_H
#define INUGAMI_TEXTURE_H

#include "inugami.hpp"

#include "image.hpp"
#include "opengl.hpp"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Inugami {

/*! @brief Handle to a texture.
 */
class Texture
{
    friend class TextureException;
public:
    /*! @brief Default constructor.
     */
    Texture() = default;

    /*! @brief Primary constructor.
     *
     *  Creates a Texture from the given Image.
     *
     *  @param img Image to upload.
     *  @param smooth Applies a smoothing filter.
     *  @param clamp Clamps texture coordinates to the image.
     */
    Texture(const Image& img, bool smooth=false, bool clamp=false);

    /*! @brief Binds the texture.
     *
     *  @param slot Texture slot to bind.
     */
    void bind(unsigned int slot) const;

    ConstAttr<int,Texture> width;   //!< Width of the texture.
    ConstAttr<int,Texture> height;  //!< Height of the texture.

private:
    class Shared
    {
    public:
        Shared();
        ~Shared();
        GLuint id;
    };

    std::shared_ptr<Shared> share;

    void upload(const Image& data, bool smooth, bool clamp);
};

} // namespace Inugami

#endif // INUGAMI_TEXTURE_H
