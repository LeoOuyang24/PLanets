#version 330 core

//shader for the gravity field of planets
//used with gravityVertexShader

#define M_PI 3.1415926535897932384626433832795




out vec4 FragColor;

in vec2 center_;
in vec2 curPos;
in float radius_;
in vec4 centerColor_;
in vec4 edgeColor_;

/*vec4 CENTER_COLOR = vec4(1,1,1,1);
vec4 EDGE_COLOR = vec4(0,1,1,0);*/

void main()
{
    float dist = length((curPos - center_)); //find the distance between interpolated pixel and center
    float angle = atan(curPos.y - center_.y, curPos.x - center_.x) + M_PI;

    float ratio = dist/(radius_*1.1);
    float angleRatio = abs(angle)/(2*M_PI);

    //FragColor.g = (length(end - start))/dist;
    /*FragColor = vec4(abs(angle)/M_PI,ratio,0,1);
    if (ratio>1 )
    {
        discard;
    }*/
    if (ratio > 1) //if beyond the radius
    {
        discard;
    }
    else
    {
        FragColor = centerColor_ + ratio*(edgeColor_ - centerColor_);

    }

}
