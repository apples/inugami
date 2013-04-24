/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex0;
uniform sampler2D noiseTex;
uniform float hue;
uniform float dissolveMin;
uniform float dissolveMax;
uniform vec3 lightPos;

out vec4 FragColor;

vec3 shiftColor(vec3 cin) {
    float c = cos(hue);
    float s = sin(hue);
    vec3 a = normalize(vec3(1.0,1.0,1.0));
    mat3 rot = mat3(
        c+a.x*a.x*(1.0-c), a.x*a.y*(1.0-c)+a.z*s, a.x*a.z*(1.0-c)-a.y*s,
        a.x*a.y*(1.0-c)-a.z*s, c+a.y*a.y*(1.0-c), a.y*a.z*(1.0-c)+a.x*s,
        a.x*a.z*(1.0-c)+a.y*s, a.y*a.z*(1.0-c)-a.x*s, c+a.z*a.z*(1.0-c)
    );
    return clamp(rot*cin, vec3(0.0,0.0,0.0), vec3(1.0,1.0,1.0));
}

void main() {
    float noise = texture( noiseTex, TexCoord ).a;

    float burn = max(max(dissolveMin-noise, noise-dissolveMax)*20.0, 0.0);
    if (burn > 1.0) discard;

    vec4 texColor;
    //burn = 0.0;
    if (burn > 0.0)
    {
        texColor = mix(vec4(1.0,0.0,0.0,1.0),vec4(0.0,0.0,0.0,1.0),burn);
    }
    else
    {
        texColor = texture(Tex0, TexCoord);
        texColor.xyz = shiftColor(texColor.xyz);
        texColor.xyz = texColor.xyz*clamp(dot(Normal, normalize(lightPos-Position)), 0.1,1.0);
    }
    FragColor = texColor;
}
