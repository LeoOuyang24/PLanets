

#include "map.h"

void StarSystem::init(int num)
{
    planets.clear();
    for (int i =0; i<num; ++i)
    {
        planets.emplace_back(new Planet());
    }
}

void StarSystem::init()
{
    init(rand()%(MAX_PLANETS - MIN_PLANETS) + MIN_PLANETS);
}
