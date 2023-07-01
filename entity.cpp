#include "entity.h"
#include "game.h"

Facing reverseFacing(Facing f)
{
    return static_cast<Facing>((static_cast<int>(f) + 1)%2);
}

glm::vec2 getPerpendicularVector(const glm::vec2& vec)
{
    return glm::vec2(-vec.y,vec.x);
}

MoveOnPlanetComponent::MoveOnPlanetComponent(const glm::vec4& rect, float speed_, Entity& entity) : BasicMoveComponent(rect,entity),
                                                                                                    ComponentContainer<MoveOnPlanetComponent>(entity),
                                                                                                    speed(speed_), baseSpeed(speed_)
{

}

MoveOnPlanetComponent::MoveOnPlanetComponent(Planet& planet, const glm::vec2& dimen, float speed_, Entity& entity) : MoveOnPlanetComponent(glm::vec4(planet.center - glm::vec2(0,planet.radius),dimen),speed_,entity)
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
    return speed;
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
                                                        speed/standingOn->radius),true); //this is the angle if we moved "speed" along the circumference
    }
    else
    {
        return center + std::min(glm::distance(target,center),speed)*betterNormalize(target - center);
    }
}

void MoveOnPlanetComponent::update()
{
    glm::vec2 center = getCenter();
    if (standingOn)
    {

        //setCenter(standingOn->getPlanetSurfacePoint(tilt,center));
        setTilt(getTiltGivenPlanet(*standingOn));
        moveCenter(standingOn->getPlanetSurfacePoint(tilt,rect)); //makes sure we are never below the surface
            PolyRender::requestLine(glm::vec4(center,standingOn->center),{0,1,1,1},1,1);

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
    BasicMoveComponent::update();
    setMovedAmount(pointDistance(lastCenter,getCenter()));
    lastCenter = getCenter();
}

void MoveOnPlanetComponent::setFacing(Facing dir)
{
    facing = dir;
}

void MoveOnPlanetComponent::addSpeed(float speed_)
{
    speed = speed_;
}

float MoveOnPlanetComponent::getMovedAmount()
{
    return movedAmount;
}

float MoveOnPlanetComponent::getTiltGivenPlanet(Planet& planet)
{
    glm::vec2 planetCenter = planet.center;
    glm::vec2 planetToPlayerVec = getCenter() - planetCenter;
    return atan2(planetToPlayerVec.y,planetToPlayerVec.x);
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
        if (MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>()) //this is important for handling "onGround"
        {
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

EntityAnimationComponent::EntityAnimationComponent(Entity& entity, BaseAnimation& anime, int fps, ZType zPos_) : BaseAnimationComponent(entity,anime, fps, zPos_), ComponentContainer<EntityAnimationComponent>(entity),
                                                                                            subSection(anime.getSubSection())
{

}

void EntityAnimationComponent::update()
{
    if (entity)
    if (MoveOnPlanetComponent* rect = entity->getComponent<MoveOnPlanetComponent>())
    {
        request(ViewPort::animeProgram,{rect->getRect(),zCoord},rect->getTilt(), rect->getFacing() == FORWARD ? NONE : MIRROR); //if we are facing backwards, mirror the sprite (this means all sprites by default need to be facing to the right
    }
    if (start == 0)
    {
        start = SDL_GetTicks();
    }
}

void EntityAnimationComponent::setSubSection(const glm::vec4& rect)
{
    subSection = rect;
}
void EntityAnimationComponent::setEffect(RenderEffect effect_)
{
    effect = effect_;
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
