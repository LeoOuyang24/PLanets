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

constexpr ZType CAMERA_Z = 1000; //max z position the camera will be at
constexpr ZType INTERFACE_Z = CAMERA_Z - 1; //Z of interface elements
constexpr ZType GAME_Z = CAMERA_Z/2; //z of game elements
constexpr ZType PLANET_Z = GAME_Z - 1; //z of planets
constexpr ZType FAR_Z = 0; //background z

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
