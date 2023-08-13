#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "render.h"

#include "planet.h"
#include "starSystem.h"
#include "entity.h"

class CursorUI
{
    BasicRenderPipeline program;
public:
    CursorUI(std::string vertex = "./shaders/dottedVertex.h", std::string fragment = "./shaders/dottedFragment.h");
    void draw(const glm::vec2& origin, const glm::vec2& mousePos);
};

class PlayerForcesComponent : public GravityForcesComponent, public ComponentContainer<PlayerForcesComponent>
{
    ForceVector moveForce = {0,0}; //force that moves the player
public:
    PlayerForcesComponent(Entity& player);
};

class PlayerControlsComponent : public Component, public ComponentContainer<PlayerControlsComponent>
{
public:
    static constexpr int MAX_FUEL = 100;
    PlayerControlsComponent(Entity& player);
    bool getMovedLastFrame();
    void update();
protected:
    int fuel = MAX_FUEL;
    bool movedLastFrame = false; //whether the player gave input to try and move last frame //REFACTOR: CURRENTLY NOT USED
    void setMovedLastFrame(bool moved_);
};

class PlayerMoveComponent : public MoveOnPlanetComponent, public ComponentContainer<PlayerMoveComponent>
{
    float velocity = 0;
public:
    static constexpr int PLAYER_DIMEN = 20;
    static constexpr float PLAYER_SPEED = 1.0f;
    static constexpr float PLAYER_SPRINT_SPEED = 2.0f;
    static constexpr float PLAYER_ACCEL = 0.1f; //amount to increase in speed by every frame
    static constexpr float PLAYER_DECEL = 0.9f; //%of speed remaining after decelerating
    static constexpr float PLAYER_IN_AIR_ACCEL = 0.1f; //acceleration if we are in the air
    PlayerMoveComponent(Entity& player);
    void setStandingOn(Planet* planet);
    void update();
};

class PlayerHealthComponent : public BaseHealthComponent, public ComponentContainer<PlayerHealthComponent>
{
    bool fallingBack = false; //"free fall". basically a stun as a result of getting hit in the air
public:
    constexpr static int PLAYER_MAX_HEALTH = 100;
    PlayerHealthComponent(Entity& player);
    void takeDamage(Entity& source, int damage); //damage is expected to be positive
    void update();
    bool getFallingBack();
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
    static Entity* createPlayer(StarSystem& system);
};


#endif // PLAYER_H_INCLUDED
