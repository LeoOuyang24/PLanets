#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "planet.h"

class StarSystem
{
    //collection of planets
    std::vector<std::unique_ptr<Planet>> planets;
    static constexpr int MIN_PLANETS = 3;
    static constexpr int MAX_PLANETS = 6;
public:
    void init(int num);
    void init();
    void update();

};

#endif // MAP_H_INCLUDED
