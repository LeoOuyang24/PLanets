#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED


#include "render.h"
#include "components.h"


typedef int SpritePath; //index of sprite, -1 for error sprite
struct Planet
{
    static RenderProgram outlineProgram;
    static constexpr int MIN_RADIUS = 10;
    static constexpr int MAX_RADIUS = 500;
    glm::vec2 center = glm::vec2(0);
    float radius = 0;
    SpritePath sprite;
    float getGravityRadius();
    void render();
};


struct PlanetSpriteManager //keeps track of every planet sprite
{
    std::vector<Sprite> sprites; //owns the sprites, make sure that PlanetSpriteManager is not deleted after GLContext
    Sprite errorMessage;
    //std::unordered_map<SpritePath,Sprite> sprites;
    void init();
    static PlanetSpriteManager MasterPlanetSprites;
    static const std::string SpritesDirectory;
    static const std::string ErrorMSG;
    static constexpr SpritePath ErrorPath = -1;
    Sprite& getSprite(SpritePath src);
    SpritePath assignSpritePath(); //get random spritePath, -1 if no sprites
private:
    PlanetSpriteManager()
    {

    }
};

#define PlanetSprites PlanetSpriteManager::MasterPlanetSprites //shorthand for PlanetSprites



#endif // PLANET_H_INCLUDED
