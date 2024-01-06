#version 330 core

//used to render gravity fields, use with gravityFragmentShader.h or starLightShader.h

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 center;
layout (location = 2) in float radius;
layout (location = 3) in vec4 centerColor; //asdf20943840928342
layout (location = 4) in vec4 edgeColor;
//layout (location = 5) in vec4 asdf;


out vec2 curPos;
out float radius_;
out vec2 center_;
out vec4 centerColor_;
out vec4 edgeColor_;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


void main()
{
    curPos = vec2(center) + aPos.xy*vec2(radius*2,-radius*2); //vertex positions
    gl_Position = projection*view*vec4(curPos,center.z,1); //screen coordination

    radius_ = radius;
    center_ = vec2(center);
    centerColor_ = centerColor;
    edgeColor_ = edgeColor;

}

