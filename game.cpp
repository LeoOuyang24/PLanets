#include "game.h"
#include "entity.h"
#include "player.h"

bool GameEntitiesManager::forEachEntity(Entity& it)
{
    EntityPosManager::forEachEntity(it);

    if (DeadComponent* dead = it.getComponent<DeadComponent>())
    if (dead->getDead())
    {
        return true;
    }
    return false;
}

GameEntitiesManager Game::manager;
StarSystem Game::solar;
std::shared_ptr<Entity> Game::player;

void Game::init()
{
    manager.init(100);
    solar.init();
    player.reset(Player::createPlayer(solar));
    Planet* planet = solar.getPlanet(0);
    manager.addEntity(player,planet->center.x, planet->center.y - planet->radius);
}

void Game::update()
{
    manager.update();
    solar.update();
    //manager.getContainer()->render();
}

EntityPosManager& Game::getManager()
{
    return manager;
}

StarSystem& Game::getSolar()
{
    return solar;
}

Entity& Game::getPlayer()
{
    return *player;
}
