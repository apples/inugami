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

#ifndef INUGAMI_LOADERS_H
#define INUGAMI_LOADERS_H

#include "inugami.hpp"

#include "mesh.hpp"

#include <string>
#include <vector>

namespace Inugami {

/*! @brief Loads text from a file.
 *
 *  The given file is loaded entirely into a string.
 *
 *  @param filename File to load.
 *
 *  @return String containing the file's data.
 *
 */
std::string loadTextFromFile(const std::string &filename);

} // namespace Inugami

#endif // INUGAMI_LOADERS_H
