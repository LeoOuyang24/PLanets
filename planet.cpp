#include <dirent.h>

#include "planet.h"

void Planet::render()
{
    SpriteManager::request(sprite,ViewPort::basicProgram,{glm::vec4(center - glm::vec2(radius,radius),radius*2,radius*2),1});
   // PolyRender::requestCircle({1,1,0,1},center,radius,true,0);
}

void PlanetSpriteManager::init()
{
    DIR* directory = opendir("./planets");

}
