#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED


#include "render.h"
#include "components.h"


typedef int SpritePath; //index of sprite, -1 for error sprite
struct Planet
{
    static std::unique_ptr<RenderProgram> outlineProgram;
    static constexpr int MIN_RADIUS = 50;
    static constexpr int MAX_RADIUS = 100;
    static const glm::vec4 CENTER_GRAVITY_COLOR; //center of gravity field color
    static const glm::vec4 EDGE_GRAVITY_COLOR; //edge of gravity field color
    glm::vec2 getPlanetSurfacePoint(float tilt, const glm::vec2& point); //move "point" so that it is "tilt" angle from the center. Maintains "point's" distance from the center of the planet, but without being inside the planet
    glm::vec2 getPlanetSurfacePoint(float tilt, const glm::vec4& rect); //adjust "rect" to be at "tilt" from the center. Returns the center of the rect
    glm::vec2 center = glm::vec2(0);
    int layer = 0;
    float radius = 0;
    SpritePath sprite;
    float getGravityRadius();
    void render();
    void renderGravityField();
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
    static std::unique_ptr<BasicRenderPipeline> GravityFieldRender; //used to render gravity fields
    static std::unique_ptr<RenderProgram> PlanetShader;

    Sprite& getSprite(SpritePath src);
    SpritePath assignSpritePath(); //get random spritePath, -1 if no sprites
private:
    PlanetSpriteManager()
    {

    }
};

#define PlanetSprites PlanetSpriteManager::MasterPlanetSprites //shorthand for PlanetSprites



#endif // PLANET_H_INCLUDED
