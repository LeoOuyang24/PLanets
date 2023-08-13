#include "entity.h"
#include "game.h"
#include "player.h"

Facing reverseFacing(Facing f)
{
    return static_cast<Facing>((static_cast<int>(f) + 1)%2);
}

float getPointAngle(const glm::vec2& point, const glm::vec2& center)
{
    glm::vec2 planetToPlayerVec = point - center;
    return atan2(planetToPlayerVec.y,planetToPlayerVec.x);
}

bool clearLine(const glm::vec2& a, const glm::vec2& b, StarSystem& system)
{
    glm::vec4 lineOfSight = glm::vec4(a,b);
    for (int i = 0; i < Game::getSolar().size(); ++i)
    {
        if (Planet* planet = Game::getSolar().getPlanet(i))
        {
            if (pointLineDistance(lineOfSight,planet->center) <= planet->radius)
            {
                return false;
            }
        }
    }
    return true;
}

float circleDistance(const glm::vec2& a, const glm::vec2& b, Planet& p)
{
    float angleA = getPointAngle(a,p.center);
    float angleB = getPointAngle(b,p.center);
    return std::min(abs(angleA - angleB), (float)abs(angleA - angleB + 2*M_PI))*p.radius;
}

glm::vec2 getPerpendicularVector(const glm::vec2& vec)
{
    return glm::vec2(-vec.y,vec.x);
}

MoveOnPlanetComponent::MoveOnPlanetComponent(const glm::vec4& rect, const MovePhysicsConstants& constants_, Entity& entity) : BasicMoveComponent(rect,entity),
                                                                                                    ComponentContainer<MoveOnPlanetComponent>(entity),
                                                                                                    constants(constants_), baseSpeed(constants_.speed)
{

}

MoveOnPlanetComponent::MoveOnPlanetComponent(Planet& planet, const glm::vec2& dimen, const MovePhysicsConstants& constants_, Entity& entity) : MoveOnPlanetComponent(glm::vec4(planet.center - glm::vec2(0,planet.radius),dimen),constants_,entity)
{
    setStandingOn(&planet);
}
void MoveOnPlanetComponent::setStandingOn(Planet* planet)
{
    standingOn = planet;
    if (planet)
    {
        setTilt(getTiltGivenPlanet(*planet));
    }
}

Planet* MoveOnPlanetComponent::getStandingOn()
{
    return standingOn;
}

float MoveOnPlanetComponent::getSpeed()
{
    return constants.speed;
}

float MoveOnPlanetComponent::getBaseSpeed()
{
    return baseSpeed;
}

float MoveOnPlanetComponent::getTilt() const
{
    return tilt + M_PI/2; //add 90 degrees so standing on top of the planet counts as "0" tilt
}

bool MoveOnPlanetComponent::getOnGround()
{
    return onGround;
}

Facing MoveOnPlanetComponent::getFacing()
{
    return facing;
}

void MoveOnPlanetComponent::moveCenter(const glm::vec2& center)
{
    setCenter(center);
    if (standingOn)
    {
        onGround = (round(glm::distance(center,standingOn->center),3) <= round(standingOn->radius + rect.a/4 ,3)) ;
        standingOn = glm::distance(center,standingOn->center) <= standingOn->getGravityRadius() + rect.a/4 ? standingOn : 0;
    }
    else
    {
        onGround = false;
    }
}

glm::vec2 MoveOnPlanetComponent::moveOnCircle(float dist, bool isAngle)
{
    glm::vec2 center = getCenter();
    if (standingOn)
    {
        float angle = getTiltGivenPlanet(*standingOn) + dist/(isAngle ? 1 : standingOn->radius); //the angle of the new point, which is "tilt" + "dist" if "isAngle" is true or
                                                                      // "dist"/"standingOn->radius" otherwise
        //PolyRender::requestCircle(glm::vec4(1,0,0,1),planetCenter + standingOn->radius*glm::vec2(cos(angle),sin(angle)),5,true,1);
        //PolyRender::requestCircle(glm::vec4(0,1,0,1),rotatePoint(center,planetCenter,dist/standingOn->radius),5,true,1);
        glm::vec2 answer = standingOn->getPlanetSurfacePoint(angle,center);// + glm::vec2(cos(tilt),sin(tilt)));
        return answer;
    }
    else
    {
        return center;//center + glm::vec2(dist,dist);
    }
}

glm::vec2 MoveOnPlanetComponent::moveOnCircle(const glm::vec2& target)
{
    glm::vec2 center = getCenter();//rotatePoint(getCenter() + glm::vec2(0,rect.a/4),center,tilt);
    if (standingOn)
    {
        glm::vec2 planetCenter = standingOn->center;
        bool clockwise = isClockwise(center,target,planetCenter); //true if moving clockwise is faster than counter clockwise.

        return moveOnCircle( (clockwise*2 - 1)*std::min(2*asin(glm::distance(target,center)/(2*standingOn->radius)), //find the minimum angle to travel, this is if we moved right onto the target
                                                        constants.speed/standingOn->radius),true); //this is the angle if we moved "speed" along the circumference
    }
    else
    {
        return center + std::min(glm::distance(target,center),constants.speed)*betterNormalize(target - center);
    }
}


void MoveOnPlanetComponent::accel(float amount)
{
    velocity = std::max(std::min(abs(constants.speed),velocity + (getOnGround()*(1 - constants.aerial_accel) + constants.aerial_accel)*amount),-1*abs(constants.speed)); //accelerate, doing it slower if we are in the air
}

void MoveOnPlanetComponent::accel(bool accelerate)
{
    accel((accelerate*2 - 1)*constants.accel);
}

void MoveOnPlanetComponent::accelMult(float amount)
{
    velocity = std::max(std::min(abs(constants.speed),velocity*amount),-1*abs(constants.speed)); //accelerate, doing it slower if we are in the air
}

void MoveOnPlanetComponent::update()
{
    glm::vec2 center = getCenter();
    if (standingOn)
    {
        moveCenter(moveOnCircle(velocity));
        setTilt(getTiltGivenPlanet(*standingOn));
        moveCenter(standingOn->getPlanetSurfacePoint(tilt,rect)); //makes sure we are never below the surface
        //PolyRender::requestLine(glm::vec4(center,standingOn->center),{0,1,1,1},1,1);

    }
    else
    {
        onGround = false;
    }
    if (!onGround)
    {
        Game::getSolar().processPlanets([this,center=getCenter()](Planet& planet){ //update standingOn

                 if (&planet != this->getStandingOn() && pointDistance(center,planet.center) <= planet.radius)
                  {
                      this->setStandingOn(&planet);
                      return true;
                  }
                    return false;
                  });
    }
    if (oldVelocity == velocity) //if we didn't increase in speed this frame
    {
        accelMult(getOnGround() ? constants.decel : .999); //apply friction
    }

    oldVelocity = velocity;

    BasicMoveComponent::update();
    setMovedAmount(pointDistance(lastCenter,getCenter()));
    lastCenter = getCenter();
}

void MoveOnPlanetComponent::setFacing(Facing dir)
{
    facing = dir;
}

void MoveOnPlanetComponent::setSpeed(float speed_)
{
    constants.speed = speed_;
}

float MoveOnPlanetComponent::getMovedAmount()
{
    return movedAmount;
}

float MoveOnPlanetComponent::getTiltGivenPlanet(Planet& planet)
{
    return getPointAngle(getCenter(),planet.center);
}

void MoveOnPlanetComponent::setMovedAmount(float amount_)
{
    movedAmount = amount_;
}

GravityForcesComponent::GravityForcesComponent(Entity& entity) : ForcesComponent(entity), ComponentContainer<GravityForcesComponent>(entity)
{

}

void GravityForcesComponent::update()
{
    if (entity)
    if (RectComponent* rect= entity->getComponent<RectComponent>())
    {
        MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>();
        if (move) //this is important for handling "onGround"
        {
            PolyRender::requestLine(glm::vec4(move->getCenter(), move->getCenter() + 10.0f*finalForce.force),{1,0,0,1},1,1);
            move->moveCenter(move->getCenter() + finalForce.force); //apply force
            if (move->getOnGround())
            {
                finalForce.force *= GROUND_FRICTION;
            }
            else
            {
                finalForce.force *= finalForce.friction;
            }
        }
        else
        {
            rect->setCenter(rect->getCenter() + finalForce.force);
            finalForce.force *= finalForce.friction;
        }
        if (!move || !move->getOnGround())
        {
        Game::getSolar().processPlanets([this,center=rect->getCenter(),rect](Planet& planet){
                          if (pointDistance(center,planet.center) <= planet.getGravityRadius())
                          {
                              glm::vec2 newForce = .0001f*(planet.radius*planet.radius)/(pow(planet.center.x - center.x,2.0f) + pow(planet.center.y - center.y,2.0f))*(planet.center - center);
                              //PolyRender::requestLine(glm::vec4(center,center + 10000.0f*newForce),{0,1,(&planet == standingOn),1},1,1);
                              addForce(newForce);
                          }
                          });
        }
    }
}

glm::vec2 GravityForcesComponent::getJumpVector(float mag, Planet* planet)
{
    if (planet)
    if (entity)
    if (RectComponent* rect = entity->getComponent<RectComponent>())
    {
        return mag*betterNormalize( rect->getCenter() - planet->center);
    }
    else
    {
        return {0,0};
    }
}

void GravityForcesComponent::setForce(const ForceVector& vec)
{
    finalForce.setForce(vec);
}

float GravityForcesComponent::getFriction()
{
    if (entity)
    if (MoveOnPlanetComponent* moveOn = entity->getComponent<MoveOnPlanetComponent>())
    {
        if (moveOn->getOnGround())
        {
            return GROUND_FRICTION;
        }
    }
    return finalForce.friction;
}

BasicSpriteComponent::BasicSpriteComponent(std::string source, Entity& entity) : RenderComponent(entity), ComponentContainer<BasicSpriteComponent>(entity)
{
    sprite.load(source);
}

void BasicSpriteComponent::update()
{
    if (entity)
    if (BasicMoveComponent* move = entity->getComponent<BasicMoveComponent>())
    {
        //PolyRender::requestRect(move->getRect(),{1,0,0,1},true,move->getTilt(),1);
        SpriteManager::request(sprite,ViewPort::basicProgram,{move->getRect(),1},move->getTilt());
    }
}

EntityAnimationComponent::EntityAnimationComponent(Entity& entity, Sprite& spriteSheet, ZType zPos_, const BaseAnimation& anime) :
                                                                                            BaseAnimationComponent(entity,spriteSheet, anime,zPos_),
                                                                                            ComponentContainer<EntityAnimationComponent>(entity)
{

}

void EntityAnimationComponent::update()
{
    if (entity)
    if (MoveOnPlanetComponent* rect = entity->getComponent<MoveOnPlanetComponent>())
    {

        PlayerHealthComponent* health = entity->getComponent<PlayerHealthComponent>();

        BaseAnimationComponent::request(Game::GameShaders.SpriteProgram,{rect->getRect(),zCoord},
                                        BaseAnimation::getFrameFromStart(start,anime),
                                        rect->getTilt(),
                                        rect->getFacing() == FORWARD ? NONE : MIRROR,//if we are facing backwards, mirror the sprite (this means all sprites by default need to be facing to the right
                                        health && health->isInvuln() ? 1.0f : 0.0f);

    }
    else if (BasicMoveComponent* rect = entity->getComponent<BasicMoveComponent>())
    {
     BaseAnimationComponent::request(Game::GameShaders.SpriteProgram,{rect->getRect(),zCoord},
                                        BaseAnimation::getFrameFromStart(start,anime),
                                        rect->getTilt(),
                                        NONE,
                                        0.0f); //if we are facing backwards, mirror the sprite (this means all sprites by default need to be facing to the right
    }
    if (start == 0)
    {
        start = SDL_GetTicks();
    }
    tint = {0,0,0,0};
}

void EntityAnimationComponent::setAnimation(const BaseAnimation& anime_)
{
    if (anime.subSection != anime_.subSection)
    {
        start = SDL_GetTicks();
        anime = anime_;
    }
}
void EntityAnimationComponent::setEffect(RenderEffect effect_)
{
    effect = effect_;
}

void EntityAnimationComponent::setTint(const glm::vec4& tint_)
{
    tint = tint_;
}

DeadComponent::DeadComponent(IsDeadCallableBase& callable, Entity& entity) : Component(entity), ComponentContainer<DeadComponent>(entity), isDead(&callable)
{

}

DeadComponent::DeadComponent(std::shared_ptr<IsDeadCallableBase>& callable, Entity& entity) : Component(entity), ComponentContainer<DeadComponent>(entity), isDead(callable)
{

}

void DeadComponent::update()
{
    bool val = ((*isDead)(entity));
    setDead(dead || val);
}

void DeadComponent::setDead(bool dead_)
{
    dead = dead_;
}

bool DeadComponent::getDead()
{
    return dead;
}

EnemyComponent::EnemyComponent(float collideDamage_, Entity& entity) : Component(entity), ComponentContainer<EnemyComponent>(entity), collideDamage(collideDamage_)
{

}

void EnemyComponent::collide(Entity& other)
{
    if (&other == &Game::getPlayer())
    {
        Game::getPlayer().getComponent<PlayerHealthComponent>()->takeDamage(*entity,collideDamage);
    }
}
