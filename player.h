#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "render.h"

#include "planet.h"
#include "starSystem.h"


class CursorUI
{
    BasicRenderPipeline program;
public:
    CursorUI(std::string vertex = "./shaders/dottedVertex.h", std::string fragment = "./shaders/dottedFragment.h");
    void draw(const glm::vec2& origin, const glm::vec2& mousePos);
};

struct Player
{
    Planet* standingOn = 0;
    bool onGround= true;
    bool airMoved = false;
    int fuel = 100;
    DeltaTime fuelRecharge;
    float speed = 1;
    glm::vec2 forces = glm::vec2(0);
    float baseSpeed = 1;
    glm::vec4 rect;
    float angle;
    CursorUI cursorUI;
    void update(StarSystem& system, RenderCamera& camera);
};


#endif // PLAYER_H_INCLUDED
