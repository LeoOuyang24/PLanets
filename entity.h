#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "physics.h"

#include "components.h"
#include "planet.h"
#include "starSystem.h"

float circleDistance(const glm::vec2& a, const glm::vec2& b, Planet& p); //returns the distance between a and b on the surface of p
float getPointAngle(const glm::vec2& point, const glm::vec2& center); //returns the angle between "point" and "center", use to calculate angle on a planet
bool clearLine(const glm::vec2& a, const glm::vec2& b, StarSystem& system); //returns true if no planet in "system" blocks the line between "a" and "b"

glm::vec2 getPerpendicularVector(const glm::vec2& vec);

enum Facing
{
    FORWARD = 0, //clockwise on a planet
    BACKWARD = 1//counterclockwise
};

Facing reverseFacing(Facing f);

struct MovePhysicsConstants
{
    //represents a collection of constants that determines how an entity moves
    static constexpr float BASE_SPEED = 1.0f;
    static constexpr float BASE_ACCEL = 0.1f;
    static constexpr float BASE_DECEL = 0.9f;
    static constexpr float BASE_AERIAL_ACCEL = 0.1f;

    float speed = BASE_SPEED; //maximum amount to move by every frame
    float accel = BASE_ACCEL; //amount to add in speed by every frame
    float decel = BASE_DECEL; //%of speed remaining after decelerating
    float aerial_accel = BASE_AERIAL_ACCEL; //acceleration if in the air
};

class MoveOnPlanetComponent : public BasicMoveComponent, public ComponentContainer<MoveOnPlanetComponent>
{
    //for moving along planets in a non-force related manner, such as walking along the surface.
public:
    MoveOnPlanetComponent(const glm::vec4& rect_, const MovePhysicsConstants& constants, Entity& entity);
    MoveOnPlanetComponent(Planet& planet, const glm::vec2& dimen, const MovePhysicsConstants& constants, Entity& entity);

    virtual void setStandingOn(Planet* planet);
    void setFacing(Facing dir);
    void setSpeed(float speed);

    Planet* getStandingOn();
    float getSpeed();
    float getBaseSpeed();
    float getTilt() const;
    bool getOnGround();
    Facing getFacing();
    float getMovedAmount();
    int getLayer();

    void moveCenter(const glm::vec2& pos); //wrapper function for setCenter. Try to use this for setting the center unless it's for something that shouldn't be affected by physics, like spawning an entity
    glm::vec2 moveOnCircle(float dist, bool angle = false); //return the new center if entity moves "dist" amount along a surface. If "angle" is true, "dist" is treated as the amount of radians to move while standing on a planet. "angle" is ignored if not standing on anything
    glm::vec2 moveOnCircle(const glm::vec2& target); //returns next center point to move to, which is either "speed" away or the target

    void accel(float amount); //increase "velocity" by "amount" up to "speed"
    void accel(bool accelerate = true); //increase "velocity" by "accelerate" if "accelerate is true, otherwise decrease
    void accelMult(float amount); //multiply "velocity" by "amount". Usually used to decelerate

    void update();
private:
    float velocity = 0;
    float oldVelocity = 0;
    MovePhysicsConstants constants;
    float baseSpeed = MovePhysicsConstants::BASE_SPEED;
    bool onGround= true;
    Planet* standingOn = 0;
    float movedAmount = 0; //how much we moved last frame
    glm::vec2 lastCenter = {0,0}; //center of our last frame
    Facing facing = FORWARD;
    int layer = 0; //the layer we are currently on.
    float getTiltGivenPlanet(Planet& planet); //returns the tilt if standing on "planet"
    void setMovedAmount(float amount_);
    void setLayer(int newLayer);

};

class GravityForcesComponent : public ForcesComponent, public ComponentContainer<GravityForcesComponent>
{
    //used for gravity physics. An entity with this component will be affected by the gravitational pull of planets
public:
    constexpr static float GROUND_FRICTION = 0.8f;
    GravityForcesComponent(Entity& player);
    void update();
    glm::vec2 getJumpVector(float mag, Planet* planet); //returns the force needed to jump if standing on "planet". "mag" is the magnitude of the vector
    void setForce(const ForceVector& vec); //sets final Force equal to vec
    float getFriction(); //returns either 'friction' or 'GROUND_FICTION', based on whether or not on ground or not
};

class BasicSpriteComponent : public RenderComponent, public ComponentContainer<BasicSpriteComponent>
{
    //really simple component, mainly for testing purpoises. Just renders a sprite
    Sprite sprite;
public:
    BasicSpriteComponent(std::string source, Entity& entity);
    void update();
};

class EntityAnimationComponent : public BaseAnimationComponent, public ComponentContainer<EntityAnimationComponent>
{
    //lets us choose which subsection of the spritesheet we want to render from
    glm::vec4 subSection;
    RenderEffect effect = NONE;
    glm::vec4 tint = {0,0,0,0};
public:
    EntityAnimationComponent(Entity&, Sprite& spriteSheet, ZType zPos = 0, const BaseAnimation& anime = {});
    /*template<typename... T>
    void request(RenderProgram& program, const FullPosition& pos, T... stuff)
    {
        SpriteManager::request(*spriteSheet,program,pos,subSection,sprite->getFramesDimen(),SDL_GetTicks() - start,fps, stuff...);
    }*/
    virtual void update();
    void setAnimation(const BaseAnimation& anime);
    void setEffect(RenderEffect effect_);
    void setTint(const glm::vec4& tint_);
};

class DeadComponent : public Component, public ComponentContainer<DeadComponent>
{
    //very simple component, checks if entity should be removed.
public:
    struct IsDeadCallableBase //parent class of IsDeadCallable, lets us have pointers to IsDeadCallable without worrying about templates
    {
        virtual bool operator()(Entity*) = 0;
    };
    template<typename Callable>
    struct IsDeadCallable : public IsDeadCallableBase
    {
        Callable func;
        virtual bool operator()(Entity* ent)
        {
            return func(ent);
        }
    };
    DeadComponent(IsDeadCallableBase& callable, Entity& entity);
    DeadComponent(std::shared_ptr<IsDeadCallableBase>& callable, Entity& entity);
    void update();
    void setDead(bool dead_);
    bool getDead();
private:
    std::shared_ptr<IsDeadCallableBase> isDead; //Callable that returns true if this entity is now dead (Entity* -> bool)
    bool dead = false;
};

class EnemyComponent : public Component, public ComponentContainer<EnemyComponent>
{
    float collideDamage = 0; //damage from colliding
public:
    EnemyComponent(float collideDamage_,Entity& entity);
    virtual void collide(Entity& other);
};

#endif // ENTITY_H_INCLUDED
