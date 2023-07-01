#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

#include "sequencer.h"
#include "components.h"

class AISequencer : public Sequencer
{

protected:
    virtual void perUnitDone();
};

struct HopAIFunc
{
    float force;
    Entity* entity;
    bool moveRight = false;
    bool jumped = false;
    HopAIFunc(Entity& entity_, float force_);
    bool operator()(int);
};

class AIComponent : public Component, public ComponentContainer<AIComponent>
{
    AISequencer states;
public:
    template<typename... Callable_>
    AIComponent(Entity& entity, Callable_&... states_) : Component(entity), ComponentContainer<AIComponent>(entity)
    {
        states.setup(states_...);
    }
    void update();
    //~AIComponent();
};

#endif // AI_H_INCLUDED
