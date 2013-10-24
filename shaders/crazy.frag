#version 330

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex0;
uniform sampler2D noiseTex;
uniform float hue;
uniform float dissolveMin;
uniform float dissolveMax;
uniform vec3 lightPos;
uniform bool useDissolve;

out vec4 FragColor;

vec3 shiftColor(vec3 cin, float h) {
    float c = cos(h);
    float s = sin(h);
    vec3 a = normalize(vec3(1.0,1.0,1.0));
    mat3 rot = mat3(
        c+a.x*a.x*(1.0-c)      ,   a.x*a.y*(1.0-c)+a.z*s,   a.x*a.z*(1.0-c)-a.y*s
        , a.x*a.y*(1.0-c)-a.z*s, c+a.y*a.y*(1.0-c)      ,   a.y*a.z*(1.0-c)+a.x*s
        , a.x*a.z*(1.0-c)+a.y*s,   a.y*a.z*(1.0-c)-a.x*s, c+a.z*a.z*(1.0-c)
    );
    return clamp(rot*cin, vec3(0.0,0.0,0.0), vec3(1.0,1.0,1.0));
}

vec4 applyLight(vec4 c)
{
    c.xyz = c.xyz*clamp(dot(normalize(Normal), normalize(lightPos-Position)), 0.1, 1.0);
    return c;
}

vec4 applyShift(vec4 c, float f)
{
    c.xyz = shiftColor(c.xyz, hue*f);
    return c;
}

vec4 applyDissolve(vec4 c)
{
    float noise = texture( noiseTex, TexCoord ).a;

    float burn = max(max(dissolveMin-noise, noise-dissolveMax)*20.0, 0.0);
    if (burn > 1.0) discard;

    if (burn > 0.0)
    {
        c = mix(vec4(1.0,0.0,0.0,1.0)*c,vec4(0.0,0.0,0.0,1.0)*c,burn);
        c = applyShift(c, 2.0);
    }
    else
    {
        c = applyLight(applyShift(c, 1.0));
    }

    return c;
}

void main() {
    vec4 texcolor = texture(Tex0, TexCoord);
    if (useDissolve) texcolor = applyDissolve(texcolor);
    else texcolor = applyLight(applyShift(texcolor, 1.0));
    FragColor = texcolor;
}
