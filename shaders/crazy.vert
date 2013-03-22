/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float wobbleX;
uniform float wobbleY;
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
void main()
{
    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
    TexCoord = VertexTexCoord;
    Normal = viewMatrix * modelMatrix*vec4(VertexNormal,0.0);
    Normal = normalize(Normal);
    Position = viewMatrix * modelMatrix*vec4(VertexPosition,1.0);
    vec4 result = MVP * vec4(VertexPosition,1.0);
    result.x = result.x * wobbleX;
    result.y = result.y * wobbleY;
    gl_Position = result;
}
