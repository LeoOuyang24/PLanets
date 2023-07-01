#ifndef STARSYSTEM_H_INCLUDED
#define STARSYSTEM_H_INCLUDED

#include "planet.h"

class StarSystem
{
    //collection of planets
    std::vector<std::unique_ptr<Planet>> planets;
    static constexpr int MIN_PLANETS = 3;
    static constexpr int MAX_PLANETS = 4;
    static constexpr int MIN_PLANETS_SPACE = 50;
    void generatePlanet(const glm::vec2& center);
public:
    void init(int num);
    void init();
    void update();
    Planet* getPlanet(int index); //returns planet at "index", null if it does not exist
    template<typename T>
    void processPlanets(T func) //pass a function that is then run on each planet. Return true if you wish to terminate early
    {
        //T = (Planet&) -> void or bool
        int i = 0;
        for (auto it = planets.begin(); it != planets.end(); ++it)
        {
            if constexpr (!std::is_same<decltype(func(std::declval<Planet&>())),bool>::value) //if the function doesn't return a bool, keep going until we've processed every planet
            {
                func(**it);
            }
            else //if the function returns bool and is true, we stop running
            {
                auto val = func(**it);
                if (val)
                {
                    return;
                }
            }
        }
    }

};

#endif //STARSYSTEM_H_INCLUDED
