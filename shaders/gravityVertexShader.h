#version 330 core

//used to render gravity fields, use with resources/shaders/simpleFragmentShader.h

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 center;
layout (location = 2) in float radius;

out vec2 curPos;
out float radius_;
out vec2 center_;

vec4 CENTER_COLOR = vec4(1,1,1,1);
vec4 EDGE_COLOR = vec4(0,0,1,1);

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
    curPos = center + aPos.xy*vec2(radius*2,-radius*2); //vertex positions
    gl_Position = projection*view*vec4(curPos,0,1); //screen coordination

    radius_ = radius;
    center_ = center;

}

