#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "render.h"

#include "planet.h"
#include "starSystem.h"
#include "entity.h"

class PlayerForcesComponent : public GravityForcesComponent, public ComponentContainer<PlayerForcesComponent>
{
public:
    PlayerForcesComponent(Entity& player);
    float getFriction();
    void update();
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
    DeltaTime warping; //time left before we warp. Unset if not warping at all
    float latchedVelocity = 0; //velocity when we latched onto a planet
    std::weak_ptr<Planet> latchedTo;
public:
    static constexpr int PLAYER_DIMEN = 10;
    static constexpr float PLAYER_SPEED = 10.0f;
    static constexpr float PLAYER_SPRINT_SPEED = 20.0f;
    static constexpr float PLAYER_ACCEL = 1; //amount to increase in speed by every frame
    static constexpr float PLAYER_DECEL = 9; //%of speed remaining after decelerating
    static constexpr float PLAYER_IN_AIR_ACCEL = 0.5f; //acceleration if we are in the air
    static constexpr int PLAYER_WARP_TIME = 20; //seconds of warping animation
    PlayerMoveComponent(Entity& player);
    void setStandingOn(Planet* planet);
    void setLatchedTo(const std::shared_ptr<Planet>& planet);
    DeltaTime & getWarping();
    Planet* getLatchedTo();
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

class PlayerAnimationComponent : public EntityAnimationComponent
{
   RenderProgram weightlessOutline;
public:
    PlayerAnimationComponent(Entity& entity, Sprite& sprite);
    void update();
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
    static Entity* createPlayer(StarSystem& system);
};


#endif // PLAYER_H_INCLUDED
