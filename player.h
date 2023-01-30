#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "planet.h"



struct Player
{
    Planet* standingOn = 0;
    bool onGround= true;
    float speed = 1;
    glm::vec2 forces = glm::vec2(0);
    float baseSpeed = 1;
    glm::vec4 rect;
    float angle;
    void update();
};

#endif // PLAYER_H_INCLUDED
