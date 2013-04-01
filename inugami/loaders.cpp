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

#include "loaders.hpp"

#include "math.hpp"
#include "mesh.hpp"
#include "utility.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <string>
#include <sstream>

namespace Inugami {

std::string loadTextFromFile(const std::string &filename)
{
    std::ifstream inFile(filename.c_str(), std::ios::binary);
    if (!inFile) throw std::runtime_error("Could not open file");

    std::string rval;
    inFile.seekg(0, std::ios::end);
    rval.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&rval[0], rval.size());

    return rval;
}

} // namespace Inugami
