#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

#include "sequencer.h"
#include "components.h"


struct AIStateBase
{
    typedef int STATE_TYPE; //type of the states

    enum States : STATE_TYPE //used to keep track of what state we are in, but also what state to move to next
    {
        NOT_DONE, //this state is not done. Returning the current state in AISequencer is equivalent
        DONE, //this state is done, don't care where we move next
        IDLE,
        PATROL,
        TARGET,
        WIND_UP, //attack wind up
        ATTACK,
        MAX_NUM_OF_STATES //number of entries in "States". Has to always be the last state in the enum
    }; //this is the "base states", basically a very basic amount of states. More advanced AIs are expected to have their own enums, which start at "MAX_NUM_OF_STATES" to avoid sharing numbers
    STATE_TYPE state = DONE;
    AIStateBase(STATE_TYPE state_) : state(state_)
    {

    }
    virtual STATE_TYPE operator()(int, Entity&) = 0; //takes in how many frames it's been, what entity is being called on, and returns which state to go to next
};

template<typename Callable>
struct AIState : public AIStateBase
{
    Callable func; //Callable is expected to be a function that returns an int, usually an enum in AIStateBase::States (int, Entity&) -> int
    AIState(AIStateBase::STATE_TYPE state_, Callable func_) : AIStateBase(state_), func(func_)
    {

    }
    virtual AIStateBase::STATE_TYPE operator()(int frames, Entity& entity)
    {
        using returnType = decltype(func(std::declval<int>(),std::declval<Entity&>()));
        if constexpr(std::is_same<returnType,bool>::value) //if Callable returns a bool, convert true to DONE and false to NOT_DONE
        {
            bool val = func(frames,entity);
            return val ? AIStateBase::DONE : AIStateBase::NOT_DONE;
        }
        return func(frames,entity);
    }
};

struct StateInfo //represents some state information,
{
    AIStateBase::STATE_TYPE state; //which state we are in
    int time; //how long it has been
};

class AISequencer
{
    typedef std::shared_ptr<AIStateBase> AIStateBasePtr;
    std::unordered_map<AIStateBase::STATE_TYPE,AIStateBasePtr> states;
    AIStateBase::STATE_TYPE currentState = AIStateBase::DONE;
    int frames = 0; //frames spent on currentState
public:
    void setCurrentState(AIStateBase::STATE_TYPE newType);
    StateInfo getCurrentState();
    void setup(AIStateBase& cs)
    {
        states[cs.state] = std::shared_ptr<AIStateBase>(&cs);
    }
    void setup(AIStateBasePtr& cs) //if you don't want the Sequencer to own the unit, pass in a shared pointer
    {
        states[cs->state] = cs;
    }
    template<typename Callable1_, typename Callable2_, typename... CallableList_>
    void setup(Callable1_& a, Callable2_& b, CallableList_&... c) //can be used to recursively pass in a list of AIStateBases or AIStateBasePtrs
    {
        setup(a);
        setup(b,c...);
    }
    void update(Entity& entity);
protected:
    //virtual void perUnitDone();
};

struct HopAIFunc
{
    float force;
    bool jumped = false;
    HopAIFunc(float);
    AIStateBase::STATE_TYPE operator()(int, Entity& entity);
};

struct ChomperAttackFunc
{
    AIStateBase::STATE_TYPE operator()(int, Entity&);
};

struct ChomperWindUpFunc
{
    int jumps = 0;
    AIStateBase::STATE_TYPE operator()(int, Entity&);

};


class AIComponent : public Component, public ComponentContainer<AIComponent>
{
    AISequencer states;
public:
    template<typename... Callable_>
    AIComponent(Entity& entity, AIStateBase::STATE_TYPE startingState, Callable_&... states_) : Component(entity), ComponentContainer<AIComponent>(entity)
    {
        states.setup(states_...);
        states.setCurrentState(startingState);
    }
    void update();
    StateInfo getState();
    //~AIComponent();
};

#endif // AI_H_INCLUDED
