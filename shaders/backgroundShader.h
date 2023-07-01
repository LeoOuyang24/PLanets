#version 330 core

//background shader, used with starlightShader.h

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 camPos;

uniform vec2 screenDimen;

out vec2 curPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
    curPos = camPos + aPos.xy*vec2(.5*screenDimen.x,-.5*screenDimen.y); //if camPos is the center of the screen, this gives us the coordinates
}
