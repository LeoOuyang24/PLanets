#ifndef STARSYSTEM_H_INCLUDED
#define STARSYSTEM_H_INCLUDED

#include "planet.h"

class StarSystem
{
    //collection of planets
    std::vector<std::unique_ptr<Planet>> planets;
    static constexpr int MIN_PLANETS = 3;
    static constexpr int MAX_PLANETS = 6;
    static constexpr int MIN_PLANETS_SPACE = 50;
    void generatePlanet(const glm::vec2& center);
public:
    void init(int num);
    void init();
    void update();
    Planet* getPlanet(int index); //returns planet at "index", null if it does not exist
    template<typename T>
    void processPlanets(T func) //pass a function that is then run on each planet.
    {
        //T = (Planet&) -> void
        for (auto it = planets.begin(); it != planets.end(); ++it)
        {
            func(**it);
        }
    }

};

#endif //STARSYSTEM_H_INCLUDED
