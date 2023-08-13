#include "AI.h"
#include "entity.h"
#include "planet.h"
#include "game.h"

/*void AISequencer::perUnitDone()
{
    //pretty much the same as a regular sequencer except we choose our next Callable randomly
    current = sequence.begin();
    std::advance(current,rand()%(sequence.size()));
    timer.reset();
    numOfReps = 0;
}*/

void AISequencer::setCurrentState(AIStateBase::STATE_TYPE newState)
{
    currentState = newState;
    frames = DeltaTime::getCurrentFrame();
}

StateInfo AISequencer::getCurrentState()
{
    return {currentState,frames};
}

void AISequencer::update(Entity& entity)
{
    if (states.find(currentState) != states.end())
    {
        AIStateBase::STATE_TYPE newState = (*(states[currentState]))(DeltaTime::getCurrentFrame() - frames,entity);
        if (newState == AIStateBase::NOT_DONE) //keep going
        {
            return;
        }
        else if (newState == AIStateBase::DONE && states.size() > 0) //state finished but doesn't care where we go next
        {
            setCurrentState((std::next(states.begin(), rand()%states.size()))->first); //choose a random state
        }
        else if (newState != currentState) //just got into a new state bitches!!!
        {
            setCurrentState(newState);
        }
    }
    else if (states.size() > 0)
    {
        setCurrentState((std::next(states.begin(), rand()%states.size()))->first); //choose a random state
    }
}

void AIComponent::update()
{
    states.update(*entity);
}

StateInfo AIComponent::getState()
{
    return states.getCurrentState();
}

/*AIComponent::~AIComponent()
{
    //states.clear();
}*/

HopAIFunc::HopAIFunc( float force_) : force(force_)
{

}

AIStateBase::STATE_TYPE HopAIFunc::operator()(int, Entity& entity)
{
    if (EntityAnimationComponent* anime = entity.getComponent<EntityAnimationComponent>())
    {
        anime->setAnimation({BaseAnimation::normalizePixels({0,0,1,246},*anime->getSpriteSheet()),5,1,5});
    }
    if (MoveOnPlanetComponent* move = entity.getComponent<MoveOnPlanetComponent>())
    {
        glm::vec2 center = move->getCenter();
        glm::vec2 playerCenter = Game::getPlayer().getComponent<RectComponent>()->getCenter();
        if (pointDistance(center,playerCenter) < 50 && clearLine(center,playerCenter,Game::getSolar()))
        {
            return AIStateBase::WIND_UP;
        }
        else
        {
            if (!jumped)
            {
                if (GravityForcesComponent* forces = entity.getComponent<GravityForcesComponent>())
                if (Planet* planet = move->getStandingOn())
                {
                  glm::vec2 jump = forces->getJumpVector(force,planet);
                  bool moveRight = rand()%2; //1 if we are jumping to the right, 0 if left
                  forces->addForce(jump + ( moveRight*2 - 1.0f)*glm::vec2(-jump.y,jump.x)); //result of jump plus the perpendicular of jump
                  move->setFacing(moveRight ? FORWARD : BACKWARD);
                  jumped = true;
                }
            }
            else
            {
                jumped = !(move->getOnGround()); //if we have landed, jumped needs to be reset to false
                return jumped ? AIStateBase::NOT_DONE : AIStateBase::IDLE;
            }
        }

    }
    return AIStateBase::NOT_DONE;

}

AIStateBase::STATE_TYPE ChomperAttackFunc::operator()(int frames, Entity& entity)
{
    if (EntityAnimationComponent* anime = entity.getComponent<EntityAnimationComponent>())
    {
        anime->setAnimation({BaseAnimation::normalizePixels({0,0,1,246},*anime->getSpriteSheet()),5,1,10});
    }
    if (MoveOnPlanetComponent* move = entity.getComponent<MoveOnPlanetComponent>())
    {
        MoveOnPlanetComponent* playerMove = Game::getPlayer().getComponent<MoveOnPlanetComponent>();
        if (move->getOnGround())
        {
            if (GravityForcesComponent* force = entity.getComponent<GravityForcesComponent>())
            {
                glm::vec2 jump = force->getJumpVector(0.5,move->getStandingOn());
                move->setFacing(isClockwise(move->getCenter(),playerMove->getCenter(),move->getStandingOn()->center) ? FORWARD : BACKWARD);
                force->addForce(jump + ((move->getFacing() == FORWARD)*2 - 1.0f)*getPerpendicularVector(jump));
                return AIStateBase::NOT_DONE;
            }
        }
        return AIStateBase::NOT_DONE;
    }
    return AIStateBase::DONE;
}

AIStateBase::STATE_TYPE ChomperWindUpFunc::operator()(int, Entity& entity)
{
    if (jumps >= 3)
    {
        return AIStateBase::ATTACK;
    }
    else if (MoveOnPlanetComponent* move = entity.getComponent<MoveOnPlanetComponent>())
    {
            move->setFacing(isClockwise(move->getCenter(),Game::getPlayer().getComponent<RectComponent>()->getCenter(),move->getStandingOn()->center) ? FORWARD : BACKWARD);
        if (EntityAnimationComponent* anime = entity.getComponent<EntityAnimationComponent>())
        {
            anime->setAnimation({BaseAnimation::normalizePixels({0,249,537,1},*anime->getSpriteSheet()),3,1,3});
        }
        if (move->getOnGround())
        {
            if (GravityForcesComponent* force = entity.getComponent<GravityForcesComponent>())
            {
                force->addForce(force->getJumpVector(1,move->getStandingOn()));
                jumps ++;
                return AIStateBase::NOT_DONE;
            }
        }
    }
    return AIStateBase::NOT_DONE;

}

