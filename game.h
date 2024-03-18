#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <unordered_map>

#include "components.h"
#include "glGame.h"

#include "starSystem.h"
#include "debug.h"

class GameEntitiesManager : public EntityPosManager
{
    bool forEachEntity(Entity& it);
public:
    /**
      *   \brief Pass a function that is then run on each Entity in the manager. If the function returns true, then it terminates early
      *
      *
      *   \param func: A "Callable", which is anything with a () operator. T = (Entity&) -> void or bool
      *
      *   \return nothing lol its a void function
      **/
    template<typename Func>
    void doForEachEntity(Func func) //run a function on each entity
    {

        doForEachElement(entities.begin(),entities.end(),[func](auto it){
                         func(*(it->first));
                         });
    }
};

struct Shaders
{
    std::unique_ptr<RenderProgram> SpriteProgram; //handles animations
    void init();
};

struct Fonts
{
    Font block;
    RenderProgram AbsoluteRendering; //renders to a fixed spot on the screen

    Fonts();
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
    static Debug debug;
    static std::shared_ptr<Fonts> fonts;
    static Shaders GameShaders;
    static void init();
    static void update();
    static GameEntitiesManager& getManager();
    static StarSystem& getSolar();
    static Entity& getPlayer();
    static ZType getCurrentZ(); //getZ of the current game, which is determined by the player's position
};

#endif // GAME_H_INCLUDED
