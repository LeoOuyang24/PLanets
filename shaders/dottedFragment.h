#version 330 core

out vec4 FragColor;

in vec4 shade;
in vec2 curPos;
flat in vec2 endPos;

uniform vec2 screenDimen;

void main()
{
    FragColor = shade;
    float dist = length(.5*(curPos - endPos)*screenDimen);
    //FragColor.g = (length(end - start))/dist;
    if (fract(dist/5) < .9)
    {
        //discard;
        //FragColor = vec4((curPos.x + 1)*.5,(curPos.y + 1)*.5,0,1);
    }
}
