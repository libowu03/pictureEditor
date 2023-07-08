#version 300 es

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec2 a_texCoord;
uniform mat4 u_matrix;

out vec2 v_texCoord;

void main()
{
    gl_Position = u_matrix * a_Position;
    v_texCoord = a_texCoord;
}