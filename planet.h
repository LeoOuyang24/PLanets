#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED


#include "render.h"
#include "components.h"

struct Planet
{
    glm::vec2 center = glm::vec2(0);
    int radius = 0;
    Sprite sprite;

    void render();
};

struct PlanetSpriteManager //keeps track of every planet sprite
{
    std::vector<Sprite> sprites;
    void init();
    static PlanetSpriteManager PlanetSprites;
private:
    PlanetSpriteManager();
};



#endif // PLANET_H_INCLUDED
