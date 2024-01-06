#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

#include "entity.h"
#include "game.h"

struct ProjectileDeathComponent : public DeadComponent, ComponentContainer<ProjectileDeathComponent>
{
    struct ProjectileDeathLambda : public IsDeadCallableBase
    {
        bool operator()(Entity* entity)
        {
            if (entity)
            if (RectComponent* rect = entity->getComponent<RectComponent>())
            {
                bool val = false;
                Game::getSolar().processPlanets([&val,rect](PlanetPtr& planet){
                                                if (pointDistance(rect->getCenter(),planet->center) <= planet->radius)
                                                {
                                                    val = true;
                                                    return true;
                                                }
                                                return false;
                                                           });
                return val;
            }
            return false;
        }
    };
    static std::shared_ptr<DeadComponent::IsDeadCallableBase> ProjectileDeathLambdaGlobal;
public:
    ProjectileDeathComponent(Entity& entity);
    void collide(Entity& other);
};

#endif // PROJECTILE_H_INCLUDED
