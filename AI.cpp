#include "AI.h"
#include "entity.h"
#include "planet.h"

void AISequencer::perUnitDone()
{
    //pretty much the same as a regular sequencer except we choose our next Callable randomly
    current = sequence.begin();
    std::advance(current,rand()%(sequence.size()));
    timer.reset();
    numOfReps = 0;
}

void AIComponent::update()
{
    states.run();
}

/*AIComponent::~AIComponent()
{
    //states.clear();
}*/

HopAIFunc::HopAIFunc(Entity& entity_, float force_) : force(force_), entity(&entity_)
{

}

bool HopAIFunc::operator()(int)
{
    if (MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>())
    {
        if (!jumped)
        {
            if (GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>())
            if (Planet* planet = move->getStandingOn())
            {
              glm::vec2 jump = forces->getJumpVector(force,planet);
              forces->addForce({jump.x - jump.y, jump.y + jump.x});
              jumped = true;
              moveRight = rand()%2;
            }
        }
        else
        {
            jumped = !(move->getOnGround()); //if we have landed, jumped needs to be reset to false
            return jumped;
        }

    }
    return false;

}


