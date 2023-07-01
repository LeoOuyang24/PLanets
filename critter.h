#ifndef CRITTER_H_INCLUDED
#define CRITTER_H_INCLUDED

#include "components.h"

class TargetComponent : public Component, public ComponentContainer<TargetComponent>
{
    //ai stuff. Lets us move towards points and what not
    glm::vec2 target = {0,0};
    bool targetIsSet = false; //if the target is actually set or not
    bool atTarget = true; //whether or not we are at the target. Does not necessarily mean our physical position is actually at the target but that is what it usually means
protected:
    virtual void updateAtTarget(); //updates atTarget to be true or false. Children classes may have different qualifications for what "being at target" means
public:
    TargetComponent(Entity& entity);
    void setTarget(const glm::vec2& target_);
    glm::vec2 getTarget();
    bool getAtTarget();
    virtual void update();
};

class RabbitAI : public TargetComponent, public ComponentContainer<RabbitAI>
{
    static constexpr int MAX_TRAVEL_DIST = 200; //max distance to travel
    static constexpr int MIN_TRAVEL_DIST = 5; //min amount of distance to travel
    static constexpr float JUMP_FORCE = 0.5f; //jump force
public:
    RabbitAI(Entity& entity);
    void update();
};

#endif // CRITTER_H_INCLUDED
