#include <dirent.h>

#include "planet.h"

RenderProgram Planet::outlineProgram;

glm::vec2 Planet::getPlanetSurfacePoint(float tilt, const glm::vec2& point)
{
    return center + std::max(glm::distance(point,center),radius)*glm::vec2(cos(tilt),sin(tilt));
}

glm::vec2 Planet::getPlanetSurfacePoint(float tilt, const glm::vec4& rect)
{
    glm::vec2 point = glm::vec2(rect.x + rect.z/2, rect.y + rect.a/2);
    return center + std::max(glm::distance(point,center),radius + rect.a/4)*glm::vec2(cos(tilt),sin(tilt));
}

void Planet::render()
{
    SpriteManager::request(PlanetSpriteManager::PlanetSprites.getSprite(sprite),ViewPort::basicProgram,{glm::vec4(center - glm::vec2(radius,radius),radius*2,radius*2),0});
    PlanetSpriteManager::GravityFieldRender.draw(GL_TRIANGLES,center,getGravityRadius());
    PolyRender::requestCircle({0,1,1,1},center,getGravityRadius(),false,0);
   //PolyRender::requestNGon(10,center,radius,{1,1,1,1},0,true,1,1);
}

float Planet::getGravityRadius()
{
    return radius*1.8f;
}

PlanetSpriteManager PlanetSpriteManager::PlanetSprites;
BasicRenderPipeline PlanetSpriteManager::GravityFieldRender;

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

    GravityFieldRender.init("./shaders/gravityVertexShader.h","./shaders/gravityFragmentShader.h",{2,1});
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
        return 0;//rand()%sprites.size();
    }
}
