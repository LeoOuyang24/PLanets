#include "resourcesMaster.h"

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

void GameEntitiesManager::addEntitiesFromLayer(int layer)
{
    clear();
    if (layerToIndex.find(layer) != layerToIndex.end())
    {
        auto it = layerToIndex[layer];
        int hsh = MasterListEntryHasher()(*it);
        while (it != masterList.end() && hsh == MasterListEntryHasher()(*it))
        {
            addEntity(*it);
            it = std::next(it);
        }
    }
}

void GameEntitiesManager::addMasterListEntry(MasterListEntry& entry)
{
    auto it = masterList.insert(entry);
    int hashbrown = MasterListEntryHasher()(entry);
    if (layerToIndex.find(hashbrown) == layerToIndex.end())
    {
        layerToIndex[hashbrown] = it;
    }

}

void GameEntitiesManager::addMasterListEntry(MasterListEntry& entry, const glm::vec2& center, int layer)
{
    if (MoveOnPlanetComponent* move = entry->getComponent<MoveOnPlanetComponent>())
    {
        move->setCenter(center);
        move->setLayer(layer);
    }
    addMasterListEntry(entry);

}

int GameEntitiesManager::size()
{
    return entities.size();
}

void GameEntitiesManager::clear()
{
    for (auto it = entities.begin(); it != entities.end();)
    {
        it = removeEntity(it->first);
    }
}

void Shaders::init()
{
    SpriteProgram = std::make_unique<RenderProgram>( "./shaders/spriteShader.h","shaders/hurtShader.h");
    //SpriteProgram->use();
    //    glUniform1i(0,GAME_Z);

}

Fonts::Fonts() : block("./block.TTF"), AbsoluteRendering({LoadShaderInfo{ResourcesConfig::config[ResourcesConfig::RESOURCES_DIR] + "/shaders/vertex/absoluteVertex.h",GL_VERTEX_SHADER,true},
                             {ResourcesConfig::config[ResourcesConfig::RESOURCES_DIR] + "/shaders/fragment/wordFragment.h",GL_FRAGMENT_SHADER,true}})
{

}


GameEntitiesManager Game::manager;
StarSystem Game::solar;
std::shared_ptr<Entity> Game::player;
Shaders Game::GameShaders;
Debug Game::debug;
std::shared_ptr<Fonts> Game::fonts;

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
    fonts.reset(new Fonts);
    Planet* planet = solar.getPlanet(0);
    manager.addMasterListEntry(player,{planet->center.x, planet->center.y - planet->radius},0);
    updateLayer(0);
}

void Game::update()
{

    debug.update();
    //std::cout << "START\n";
    //player->getComponent<PlayerForcesComponent>()->update();
    manager.update();
    //std::cout << "ASDF\n";
    solar.update();
    renderInterface();
    //manager.getContainer()->render();
}

GameEntitiesManager& Game::getManager()
{
    return manager;
}

StarSystem& Game::getSolar()
{
    return solar;
}

Entity& Game::getPlayer()
{
    return *player; //REFACTOR: DANGEROUS IF PLAYER DOES NOT YET EXIST
}

ZType Game::getCurrentZ()
{
   if (player.get())
   if (MoveOnPlanetComponent* move = player->getComponent<MoveOnPlanetComponent>())
   {
       return getSolar().getZGivenLayer(move->getLayer());
   }
   return GAME_Z;
}

void Game::updateLayer(int layer)
{
    manager.addEntitiesFromLayer(layer);
    manager.addEntity(player);
}
