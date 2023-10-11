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

void Shaders::init()
{
    SpriteProgram.init("shaders/spriteShader.h","shaders/hurtShader.h",{4,1,4,1,1,1});
}

GameEntitiesManager Game::manager;
StarSystem Game::solar;
std::shared_ptr<Entity> Game::player;
Shaders Game::GameShaders;

void Game::renderInterface()
{
    if (BaseHealthComponent* health = player->getComponent<BaseHealthComponent>())
    {
        PolyRender::requestRect(ViewPort::toAbsolute({10,10,health->getHealth()/health->getMaxHealth()*100,10}),{1,0,0,1},true,0,ViewPort::currentCamera->getPos().z - 1);
    }
    else
    {
        std::cerr << "Game::renderInterface: PLAYER IS MISSING HEALTH COMPONENT\n";
    }
}

void Game::init()
{
    manager.init(100);
    solar.init();
    GameShaders.init();
    player.reset(Player::createPlayer(solar));
    Planet* planet = solar.getPlanet(0);
    manager.addEntity(player,planet->center.x, planet->center.y - planet->radius);
}

void Game::update()
{
    manager.update();
    solar.update();
    renderInterface();
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
