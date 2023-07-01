#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <unordered_map>

#include "components.h"
#include "glGame.h"

#include "starSystem.h"

class GameEntitiesManager : public EntityPosManager
{
    bool forEachEntity(Entity& it);
};

class Game
{
    static GameEntitiesManager manager;
    static StarSystem solar;
    static std::shared_ptr<Entity> player;
public:
    static void init();
    static void update();
    static EntityPosManager& getManager();
    static StarSystem& getSolar();
    static Entity& getPlayer();
};

#endif // GAME_H_INCLUDED
