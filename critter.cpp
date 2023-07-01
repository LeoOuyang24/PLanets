#include "critter.h"
#include "entity.h"

void TargetComponent::updateAtTarget()
{
    if (entity)
    {
        BasicMoveComponent* move = entity->getComponent<BasicMoveComponent>();
        atTarget = !targetIsSet || !move || (pointDistance(move->getCenter(),target)<= 1.0f);
    }
}

TargetComponent::TargetComponent(Entity& entity_) : Component(entity_), ComponentContainer<TargetComponent>(entity_)
{

}

void TargetComponent::setTarget(const glm::vec2& target_)
{
    target = target_;
    targetIsSet = true;
}

glm::vec2 TargetComponent::getTarget()
{
    return target;
}

bool TargetComponent::getAtTarget()
{
    return atTarget;
}

void TargetComponent::update()
{
    updateAtTarget();
}

RabbitAI::RabbitAI(Entity& entity) : TargetComponent(entity),ComponentContainer<RabbitAI>(entity)
{

}

void RabbitAI::update()
{
    updateAtTarget();
    if (entity)
    if (MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>())
    if (GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>())
    {
        if (getAtTarget()) //already at target, set a new target
        {
            glm::vec2 newPoint =move->moveOnCircle((rand()%2*2-1)*(rand()%(MAX_TRAVEL_DIST - MIN_TRAVEL_DIST) + MIN_TRAVEL_DIST));
            setTarget(newPoint);
        }
        else
        {
            if (move->getOnGround())
            {
                forces->addForce(forces->getJumpVector(JUMP_FORCE,move->getStandingOn()));
            }
            move->moveCenter(move->moveOnCircle(getTarget()));
        }
    }
    PolyRender::requestCircle(glm::vec4(1,0,0,1),getTarget(),5,true,2);
}
