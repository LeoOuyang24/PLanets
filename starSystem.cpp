

#include "starSystem.h"

void StarSystem::generatePlanet(const glm::vec2& center)
{
    Planet* planet = new Planet();
    planet->center = center;
    planet->radius = 500;//rand()%(Planet::MAX_RADIUS - Planet::MIN_RADIUS) + Planet::MIN_RADIUS;
    planet->sprite = PlanetSprites.assignSpritePath();
    //std::cout << planet->sprite << "\n";
    planets.emplace_back(planet);
}

void StarSystem::init(int num)
{
    planets.clear();
    for (int i =0; i<num; ++i)
    {
        generatePlanet({(i + 1)*(2*Planet::MAX_RADIUS + MIN_PLANETS_SPACE),(i + 1)*(2*Planet::MAX_RADIUS + MIN_PLANETS_SPACE)});
    }
}

void StarSystem::init()
{
    if constexpr(MAX_PLANETS == MIN_PLANETS)
    {
        init(MAX_PLANETS);
    }
    else
    {
        init(rand()%(MAX_PLANETS - MIN_PLANETS) + MIN_PLANETS);
    }
}

void StarSystem::update()
{
    for (auto it = planets.begin(); it != planets.end(); ++it)
    {
        (*it)->render();
    }
}

Planet* StarSystem::getPlanet(int index)
{
    if (index < 0 || index >= planets.size())
    {
        return 0;
    }
    return planets[index].get();
}
