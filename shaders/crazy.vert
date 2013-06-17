#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 MVP;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;
    Normal = viewMatrix * modelMatrix*vec4(VertexNormal,0.0);
    Normal = normalize(Normal);
    Position = viewMatrix * modelMatrix*vec4(VertexPosition,1.0);
    vec4 result = MVP * vec4(VertexPosition,1.0);
    gl_Position = result;
}
