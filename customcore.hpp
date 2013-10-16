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

#ifndef CUSTOMCORE_H
#define CUSTOMCORE_H

#include "inugami/core.hpp"

#include "inugami/animatedsprite.hpp"
#include "inugami/mesh.hpp"
#include "inugami/shader.hpp"
#include "inugami/spritesheet.hpp"
#include "inugami/texture.hpp"

class CustomCore : public Inugami::Core
{
public:
    CustomCore(const RenderParams &params);
    virtual ~CustomCore();

    void tick();
    void draw();

private:
    float rotation;
    float dissolveMin, dissolveMax;
    unsigned int ticks;
    bool highDef;
    bool shaderOn;

    Inugami::Texture        shieldTex;
    Inugami::Texture        noiseTex;
    Inugami::AnimatedSprite fontRoll;
    Inugami::Mesh           shield;
    Inugami::Mesh           shieldHD;
    Inugami::Shader         defaultShader;
    Inugami::Shader         crazyShader;
};

#endif // CUSTOMCORE_H
