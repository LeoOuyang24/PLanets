#include "projectile.h"
#include "game.h"

std::shared_ptr<DeadComponent::IsDeadCallableBase> ProjectileDeathComponent::ProjectileDeathLambdaGlobal = std::make_shared<ProjectileDeathComponent::ProjectileDeathLambda>();

ProjectileDeathComponent::ProjectileDeathComponent(Entity& entity) : DeadComponent(ProjectileDeathComponent::ProjectileDeathLambdaGlobal,entity), ComponentContainer<ProjectileDeathComponent>(entity)
{

}

void ProjectileDeathComponent::collide(Entity& other)
{
    if (&other != &Game::getPlayer())
    {
        setDead(true);
    }
}
