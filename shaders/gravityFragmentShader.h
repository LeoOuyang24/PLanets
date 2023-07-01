#version 330 core

//shader for the gravity field of planets
//used with gravityVertexShader

out vec4 FragColor;

in vec2 center_;
in vec2 curPos;
in float radius_;

vec4 CENTER_COLOR = vec4(1,1,1,1);
vec4 EDGE_COLOR = vec4(0,1,1,0);

void main()
{
    float dist = length((curPos - center_)); //find the distance between interpolated pixel and center
    float ratio = dist/(radius_*1.1);

    //FragColor.g = (length(end - start))/dist;
    FragColor = CENTER_COLOR + ratio*(EDGE_COLOR - CENTER_COLOR);
    if (ratio > 1) //if beyond the radius
    {
        discard;
    }
}
