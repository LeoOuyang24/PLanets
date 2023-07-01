#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "physics.h"

#include "components.h"
#include "planet.h"
#include "starSystem.h"

glm::vec2 getPerpendicularVector(const glm::vec2& vec);

enum Facing
{
    FORWARD = 0, //clockwise on a planet
    BACKWARD = 1//counterclockwise
};

Facing reverseFacing(Facing f);

class MoveOnPlanetComponent : public BasicMoveComponent, public ComponentContainer<MoveOnPlanetComponent>
{
    //for moving along planets in a non-force related manner, such as walking along the surface.
public:
    MoveOnPlanetComponent(const glm::vec4& rect_, float speed_, Entity& entity);
    MoveOnPlanetComponent(Planet& planet, const glm::vec2& dimen, float speed_, Entity& entity);

    virtual void setStandingOn(Planet* planet);
    void setFacing(Facing dir);
    void addSpeed(float speed);

    Planet* getStandingOn();
    float getSpeed();
    float getBaseSpeed();
    float getTilt() const;
    bool getOnGround();
    Facing getFacing();
    float getMovedAmount();

    void moveCenter(const glm::vec2& pos); //wrapper function for setCenter. Try to use this for setting the center unless it's for something that shouldn't be affected by physics, like spawning an entity
    glm::vec2 moveOnCircle(float dist, bool angle = false); //return the new center if entity moves "dist" amount along a surface. If "angle" is true, "dist" is treated as the amount of radians to move while standing on a planet. "angle" is ignored if not standing on anything
    glm::vec2 moveOnCircle(const glm::vec2& target); //returns next center point to move to, which is either "speed" away or the target
    void update();
private:
    float getTiltGivenPlanet(Planet& planet); //returns the tilt if standing on "planet"
    void setMovedAmount(float amount_);
    Planet* standingOn = 0;
    bool onGround= true;
    float speed = 1;
    float baseSpeed = 1;
    float movedAmount = 0; //how much we moved last frame
    glm::vec2 lastCenter = {0,0}; //center of our last frame
    Facing facing = FORWARD;
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
public:
    EntityAnimationComponent(Entity&, BaseAnimation& anime, int fps = 1, ZType zPos = 0);
    template<typename... T>
    void request(RenderProgram& program, const FullPosition& pos, T... stuff)
    {
        SpriteManager::request(*sprite,program,pos,subSection,sprite->getFramesDimen(),SDL_GetTicks() - start,fps, stuff...);
    }
    void update();
    void setSubSection(const glm::vec4& rect);
    void setEffect(RenderEffect effect_);
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

#endif // ENTITY_H_INCLUDED
