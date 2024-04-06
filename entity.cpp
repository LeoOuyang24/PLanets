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
                                                                                                    constants(constants_)
{

}

MoveOnPlanetComponent::MoveOnPlanetComponent(Planet& planet, const glm::vec2& dimen, const MovePhysicsConstants& constants_, Entity& entity) : MoveOnPlanetComponent(glm::vec4(planet.center - glm::vec2(0,planet.radius),dimen),constants_,entity)
{
    setStandingOn(&planet);
}
void MoveOnPlanetComponent::setStandingOn(Planet* planet)
{
    //if (entity == &Game::getPlayer() && planet && pointDistance(getCenter(),planet->center) > 500 && planet->layer == layer)
    prevStandingOn = standingOn;
    standingOn = planet;
    //onGround = true;
    if (planet)
    {
        setLayer(planet->layer);
    }
}

void MoveOnPlanetComponent::teleportToPlanet(Planet& planet)
{
    setStandingOn(&planet);
    setCenter(planet.getPlanetSurfacePoint(getTiltGivenPlanet(planet),rect));
    //setCenter(planet.center + planet.radius*glm::vec2(cos(getTilt()),sin(getTilt())));
}

Planet* MoveOnPlanetComponent::getStandingOn()
{
    return standingOn;
}

Planet* MoveOnPlanetComponent::getPrevStandingOn()
{
    return prevStandingOn;
}

float MoveOnPlanetComponent::getSpeed()
{
    return constants.speed;
}

float MoveOnPlanetComponent::getVelocity()
{
    return velocity;
}

void MoveOnPlanetComponent::setVelocity(float vel)
{
    velocity =  std::max(std::min(constants.speed,vel),-1*constants.speed); //accelerate, ensuring it never bypasses our speed. We also round to two decimal points for simplicity
    //std::cout << "VEL: " << velocity << "\n";
}

float MoveOnPlanetComponent::getTiltOnPlanet() const
{
    return tilt + M_PI/2;
}

bool MoveOnPlanetComponent::getOnGround()
{
    return onGround;
}

Facing MoveOnPlanetComponent::getFacing()
{
    return facing;
}

glm::vec2 MoveOnPlanetComponent::getMoveVector()
{
    return getCenter() - lastCenter;
}

float MoveOnPlanetComponent::getTiltGivenPlanet(Planet& planet)
{
    return getPointAngle(getCenter(),planet.center);
}

int MoveOnPlanetComponent::getLayer()
{
    return layer;
}

void MoveOnPlanetComponent::moveCenter(const glm::vec2& center)
{
    setCenter(center);
    if (standingOn)
    {
        onGround = (round(glm::distance(center,standingOn->center),3) <= round(standingOn->radius + rect.a/4 ,3)) ;
        //PolyRender::requestCircle({1,0,0,1},standingOn->center,standingOn->getGravityRadius(),true,StarSystem::getPlanetZGivenLayer(standingOn->layer));
        //setStandingOn(glm::distance(center,standingOn->center) <= standingOn->getGravityRadius() + rect.a/4 ? standingOn : 0);

    }
    else
    {
        onGround = false;
    }
}

glm::vec2 MoveOnPlanetComponent::moveOnCircle(float dist, bool isAngle)
{
    glm::vec2 center = getCenter();
    GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>();

    if (standingOn)
    if (entity)
    {
        float angle = getTiltGivenPlanet(*standingOn) + dist/(isAngle ? 1 : pointDistance(center,standingOn->center)); //the angle of the new point, which is "tilt" + "dist" if "isAngle" is true or
                                                                      // "dist"/"standingOn->radius" otherwise
        //PolyRender::requestCircle(glm::vec4(1,0,0,1),planetCenter + standingOn->radius*glm::vec2(cos(angle),sin(angle)),5,true,1);
        //PolyRender::requestCircle(glm::vec4(0,1,0,1),rotatePoint(center,planetCenter,dist/standingOn->radius),5,true,1);
        glm::vec2 answer = standingOn->getPlanetSurfacePoint(angle,center);// + glm::vec2(cos(tilt),sin(tilt)));
        return answer;
    }
    else
    {
        return center + dist*glm::vec2(cos(getTiltOnPlanet()),sin(getTiltOnPlanet()));
    }
    return center;
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


void MoveOnPlanetComponent::accel(bool accelerate)
{
    //std::cout << (accelerate*2 - 1)*(getOnGround() ? constants.accel : constants.aerial_accel)<<"\n";
   // if (standingOn && pointDistance(getCenter(),standingOn->center) <= standingOn->getGravityRadius())
    {
        setVelocity(velocity + (accelerate*2 - 1)*(getOnGround() ? constants.accel : constants.aerial_accel)); //accelerate, slower in the air
    }
}

void MoveOnPlanetComponent::accelMult(float amount)
{
    setVelocity(velocity*amount);
}

void MoveOnPlanetComponent::update()
{
    glm::vec2 center = getCenter();
    setFacing(velocity < 0 ? BACKWARD : FORWARD);
    if (entity)
    if (GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>())
    {
        forces->setForce(moveOnCircle(velocity) - center,SELF);

        //forces->addForce(moveOnCircle(convertTo1(velocity)*constants.accel) - center, SELF);
        if (velocity == oldVelocity) //if we didn't change our velocity, time to apply friction. This is usually the result of our entity not moving anymore
        {
            //std::cout << "frictining " << forces->getFriction() << "\n";
            accelMult(forces->getFriction());
        }

    }

    if (standingOn)
    {
        //std::cout << atan2(0,1) << " " << getTiltGivenPlanet(*standingOn) << "\n";
        setTilt(getTiltGivenPlanet(*standingOn));
        moveCenter(standingOn->getPlanetSurfacePoint(getTiltGivenPlanet(*standingOn),rect)); //makes sure we are never below the surface
        onGround = (round(glm::distance(center,standingOn->center),3) <= round(standingOn->radius + rect.a/4 ,3)) ;

        //PolyRender::requestLine(glm::vec4(center,standingOn->center),{0,1,1,1},1,1);

    }
    else
    {
        onGround = false;
    }
    if (!onGround)
    {
        Game::getSolar().processPlanets([this,center=getCenter()](PlanetPtr& planet){ //update standingOn

                 if (planet.get() != this->getStandingOn() && pointDistance(center,planet->center) <= planet->radius)
                  {
                      this->setStandingOn(planet.get());
                      return true;
                  }
                    return false;
                  },layer);
    }
    else
    {
        prevStandingOn = nullptr;
    }

    oldVelocity = velocity;

    BasicMoveComponent::update();
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

void MoveOnPlanetComponent::setLayer(int newLayer_)
{
    layer = newLayer_;
}


GravityForcesComponent::GravityForcesComponent(Entity& entity) : Component(entity), ComponentContainer<GravityForcesComponent>(entity)
{

}

void GravityForcesComponent::addForce(const glm::vec2& force, ForceSource source) //adds "force" to the force in "forces". If the source has not been added yet, create it
{
    if (forces.find(source) == forces.end())
    {
        setForce(force,source);
    }
    else
    {
        setForce(force + forces[source],source);
    }
}

void GravityForcesComponent::setForce(glm::vec2 force, ForceSource source)
{

    float length = glm::length(force);

    if (length <= .001)
    {
        force = glm::vec2(0);
    }
    else if (length > MAX_FORCE_MAG)
    {
        force = MAX_FORCE_MAG*(1/length)*(force);
    }

    forces[source] = force;

    //std::cout << glm::length(forces[source]) << "\n";
}

void GravityForcesComponent::applyFriction(float friction, ForceSource source)
{
    if (forces.find(source) != forces.end())
    {
        setForce(friction*forces[source],source);

    }
}

void GravityForcesComponent::applyFrictionAll(float friction)
{
    for (auto it = forces.begin(); it != forces.end(); ++it)
    {
        applyFriction(friction,it->first);
    }
}

const std::unordered_map<ForceSource,Force>& GravityForcesComponent::getAllForces()
{
    return forces;
}

Force GravityForcesComponent::getForce(ForceSource source)
{
    if (forces.find(source) == forces.end())
    {
        return {0,0};
    }
    return forces[source];
}

Force GravityForcesComponent::getTotalForce()
{
    Force finalForce = glm::vec2(0);
    for_each(forces.begin(), forces.end(), [&finalForce](const std::pair<ForceSource,Force>& f){finalForce += f.second;});
    return finalForce;
}

void GravityForcesComponent::update()
{
    //std::cout << glm::length(forces[SELF]) << "\n";
    if (entity)
    if (RectComponent* rect= entity->getComponent<RectComponent>())
    {
        glm::vec2 finalForce = getTotalForce();
        MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>();
        if (move) //this is important for handling "onGround"
        {
            //PolyRender::requestLine(glm::vec4(move->getCenter(), move->getCenter() + 10.0f*finalForce.force),{1,0,0,1},1,1);
            move->moveCenter(move->getCenter() + GLOBAL_FORCE_PORTION*finalForce); //apply force
            applyFrictionAll(getFriction());

        }
        else
        {
            rect->setCenter(rect->getCenter() + finalForce);
            applyFrictionAll(getFriction());
        }
        if ((!move || !move->getOnGround()) && !weightless)
        {
            glm::vec2 center = move->getCenter();
        Game::getSolar().processPlanets([this,center,rect,move](PlanetPtr& planet){
                          if (pointDistance(center,planet->center) <= planet->getGravityRadius() && planet.get() != move->getStandingOn() && planet.get() != move->getPrevStandingOn())
                          {
                              move->setStandingOn(planet.get());
                             /* setForce(getForce(SELF),MISC);
                            setForce({0,0},GRAVITY);*/
                            glm::vec2 jump = getForce(JUMP);
                            glm::vec2 self = getForce(SELF);
                            glm::vec2 force = getTotalForce();
                            float gravMag = glm::length(getForce(GRAVITY));
                            applyFrictionAll(0);
                            addForce(force,MISC);
                            move->setVelocity(0);

                              return true;
                              //inSpace = false;
                          }
                          return false;
                          },move->getLayer());
          if (Planet* planet = move->getStandingOn())
          {
             glm::vec2 newForce = .005f*(planet->radius*planet->radius)/(pow(planet->center.x - move->getCenter().x,2.0f) + pow(planet->center.y - move->getCenter().y,2.0f))*(planet->center - center);
            addForce(newForce,GRAVITY);
          }


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
    return {0,0};

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
    return BASE_FRICTION;
}

void GravityForcesComponent::setWeightless(bool weightless_)
{
    weightless = weightless_;
}

bool GravityForcesComponent::getWeightless()
{
    return weightless;
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
        SpriteManager::requestSprite({*ViewPort::basicProgram,&sprite},move->getRect(),1,move->getTilt());
    }
}

EntityAnimationComponent::EntityAnimationComponent(Entity& entity, Sprite& spriteSheet,  const BaseAnimation& anime) :
                                                                                            BaseAnimationComponent(entity,spriteSheet, anime,0),
                                                                                            ComponentContainer<EntityAnimationComponent>(entity)
{

}

void EntityAnimationComponent::update()
{
    if (entity)
    if (MoveOnPlanetComponent* rect = entity->getComponent<MoveOnPlanetComponent>())
    {

        BaseHealthComponent* health = entity->getComponent<BaseHealthComponent>();

        BaseAnimationComponent::request(*Game::GameShaders.SpriteProgram,rect->getRect(),StarSystem::getZGivenLayer(rect->getLayer()),
                                        BaseAnimation::getFrameFromStart(start,anime),
                                        rect->getTiltOnPlanet(),
                                        rect->getFacing() == FORWARD ? NONE : MIRROR,//if we are facing backwards, mirror the sprite (this means all sprites by default need to be facing to the right
                                        health && health->isInvuln() ? 1.0f : 0.0f);

    }
    else if (BasicMoveComponent* rect = entity->getComponent<BasicMoveComponent>())
    {
     BaseAnimationComponent::request(*Game::GameShaders.SpriteProgram,rect->getRect(),zCoord,
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

WarpingComponent::WarpingComponent(Entity& entity) : Component(entity), ComponentContainer<WarpingComponent>(entity)
{

}

void WarpingComponent::setWarping(PlanetIndex target)
{
    warpTo = target;
    timeToWarp.set();
}

bool WarpingComponent::isWarping()
{
    return timeToWarp.isSet();
}

void WarpingComponent::update()
{
    if (timeToWarp.getTimePassed() >= WARP_TIME)
    if (Planet* pl = Game::getSolar().getPlanet(warpTo))
    if (entity)
    if (MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>())
    {
        move->teleportToPlanet(*pl);
        timeToWarp.reset();
    }
}
