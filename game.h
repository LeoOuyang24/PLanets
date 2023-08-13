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

struct Shaders
{
    RenderProgram SpriteProgram; //handles animations
    void init();
};

class Game
{
    static GameEntitiesManager manager;
    static StarSystem solar;
    static std::shared_ptr<Entity> player;
    static void renderInterface();
public:
    static Shaders GameShaders;
    static void init();
    static void update();
    static EntityPosManager& getManager();
    static StarSystem& getSolar();
    static Entity& getPlayer();
};

#endif // GAME_H_INCLUDED
