

#include "starSystem.h"
#include "game.h"
#include "entity.h"

int StarSystem::getPlanetZGivenLayer(int layer)
{
    return layer*LAYER_SPACE*-1;
}

int StarSystem::getLayerGivenPlanet(Planet& planet)
{
    return planet.z/LAYER_SPACE;
}

Planet& StarSystem::generatePlanet(const glm::vec2& center)
{
    Planet* planet = new Planet();
    planet->center = center;
    planet->radius = rand()%(Planet::MAX_RADIUS - Planet::MIN_RADIUS + 1) + Planet::MIN_RADIUS;
    planet->sprite = PlanetSprites.assignSpritePath();
    //std::cout << planet->sprite << "\n";
    //planets.emplace_back(planet);
    return *planet;
}

void StarSystem::insertPlanet(Planet& planet, int layer)
{
    planet.z = getPlanetZGivenLayer(layer);
    if (layer < layers.size())
    {
        planets.insert(planets.begin() + layers[layer],std::unique_ptr<Planet>(&planet));
    }
    else
    {
        layers.push_back(planets.size()); //add the new layer
        planets.insert(planets.begin() + planets.size(), std::unique_ptr<Planet>(&planet)); //insert into next layer
    }
}

void StarSystem::init()
{
    planets.clear();
    layers.clear();
    int numOfLayers = rand()%3 + 2;
    for (int i =0; i<numOfLayers; ++i)
    {
        int layerSize = rand()%(MAX_PLANETS - MIN_PLANETS + 1) + MIN_PLANETS;
        int offset = planets.size(); //number of planets not counting this layer
        for (int j = 0; j < layerSize; ++j)
        {
            insertPlanet(generatePlanet({(j + 1)*(Planet::MAX_RADIUS + MIN_PLANETS_SPACE),(j + 1)*(Planet::MAX_RADIUS + MIN_PLANETS_SPACE)}),i);
        }
    }
}


void StarSystem::update()
{
    /*
    for (PlanetIndex it = 0; it < planets.size(); ++it)
    {
        (planets[it])->render();
    }*/
    processPlanets([](Planet& planet){
                   planet.render();

                   });
    processPlanets([](Planet& planet){
                    if (getLayerGivenPlanet(planet) == Game::getPlayer().getComponent<MoveOnPlanetComponent>()->getLayer())
                   {
                       planet.renderGravityField();
                   }
                   });
}

size_t StarSystem::size()
{
    return planets.size();
}

Planet* StarSystem::getPlanet(int index)
{
    if (index < 0 || index >= planets.size())
    {
        return 0;
    }
    return planets[index].get();
}
