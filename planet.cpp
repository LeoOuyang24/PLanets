#include <dirent.h>

#include "planet.h"

void Planet::render()
{
    SpriteManager::request(PlanetSpriteManager::PlanetSprites.getSprite(sprite),ViewPort::basicProgram,{glm::vec4(center - glm::vec2(radius,radius),radius*2,radius*2),1});
    PolyRender::requestCircle({0,1,1,1},center,getGravityRadius(),false,0);
}

float Planet::getGravityRadius()
{
    return radius*1.5f;
}

PlanetSpriteManager PlanetSpriteManager::PlanetSprites;

const std::string PlanetSpriteManager::SpritesDirectory = "./planets/";
const std::string PlanetSpriteManager::ErrorMSG = "./sprites/errorMSG.png";
void PlanetSpriteManager::init()
{
    DIR* directory = opendir(SpritesDirectory.c_str());
    if (directory == 0)
    {
        throw std::runtime_error("UH OH FOR WHATEVER REASON PLANET SPRITES DIRECTORY COULD NOT BE FOUND.");
    }
    for (struct dirent *dent; (dent = readdir(directory)) != NULL; ) //go through each file and add it as a sprite
    {
        const char *filename = dent->d_name;

        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)
            {
                //sprites[SpritesDirectory + std::string(filename)].load(SpritesDirectory + filename);
                sprites.emplace_back(SpritesDirectory + filename);
            }
    }
    errorMessage.load(ErrorMSG);
}

Sprite& PlanetSpriteManager::getSprite(SpritePath src)
{
    if (src >= sprites.size() || src < 0)//(it != sprites.end())
    {
        return errorMessage;
    }
    else
    {
        return sprites[src];
        /*it = sprites.find(ErrorMSG);
        if (it != sprites.end())
        {
            std::cerr << "Failed to load sprite: " << src << "\n";
            return sprites[ErrorMSG];
        }
        else
        {
            throw std::runtime_error("Failed to load sprite " + src + " AND the error message. Something has gone really wrong. Did you call PlanetSpriteManager.PlanetSprites::init?");

        }*/
    }
}

SpritePath PlanetSpriteManager::assignSpritePath()
{
    if (sprites.size() == 0)
    {
        return ErrorPath;
    }
    else
    {
        return rand()%sprites.size();
    }
}
