#version 330 core

out vec4 FragColor;

in vec2 curPos;

uniform float radius;
uniform vec2 origin;
uniform vec4 starColor;
uniform vec4 voidColor;

void main()
{
    float dist = length((curPos - origin));
    float ratio = dist/radius;
    if (ratio > 1.0)
    {
        ratio = 1.0;
    }
    //FragColor.g = (length(end - start))/dist;
    FragColor = starColor + ratio*(voidColor - starColor);
}
