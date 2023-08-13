

#include "starSystem.h"

void StarSystem::generatePlanet(const glm::vec2& center)
{
    Planet* planet = new Planet();
    planet->center = center;
    planet->radius = rand()%(Planet::MAX_RADIUS - Planet::MIN_RADIUS + 1) + Planet::MIN_RADIUS;
    planet->sprite = PlanetSprites.assignSpritePath();
    //std::cout << planet->sprite << "\n";
    planets.emplace_back(planet);
}

void StarSystem::init(int num)
{
    planets.clear();
    for (int i =0; i<num; ++i)
    {
        generatePlanet({(i + 1)*(Planet::MAX_RADIUS + MIN_PLANETS_SPACE),(i + 1)*(Planet::MAX_RADIUS + MIN_PLANETS_SPACE)});
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
        init(rand()%(MAX_PLANETS - MIN_PLANETS + 1) + MIN_PLANETS); //+1 because otherwise it would not be possible to generate "MAX_PLANETS" planets
    }
}

void StarSystem::update()
{
    for (auto it = planets.begin(); it != planets.end(); ++it)
    {
        (*it)->render();
    }
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
