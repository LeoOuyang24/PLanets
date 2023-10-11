#ifndef STARSYSTEM_H_INCLUDED
#define STARSYSTEM_H_INCLUDED

#include "planet.h"

class StarSystem
{
    //collection of planets
    typedef std::vector<std::unique_ptr<Planet>> Planets;
    typedef int PlanetIndex;
    Planets planets; //the planets in this system. The planets are expected to be ordered based on which layer they are in (a planet in a layer closer to the screen will always be before a planet closer to the sun)
    std::vector<PlanetIndex> layers; //first indicies in "planets" that denote the beginning of a new layer. Layers are ordered based on how close they are to the screen, (smaller is closer)
    //std::set<LayerType> layers;
    static constexpr int MIN_PLANETS = 1;
    static constexpr int MAX_PLANETS = 3;
    static constexpr int MIN_PLANETS_SPACE = 50;
    static constexpr int LAYER_SPACE = 300; //space between the layers
    Planet& generatePlanet(const glm::vec2& center);
    void insertPlanet(Planet& planet, int layer); //insert "planet" at the "layer"-th layer
public:
    static int getPlanetZGivenLayer(int layer); // given a layer, get the corresponding z coordinate
    static int getLayerGivenPlanet(Planet& planet); //given a planet, get the corresponding layer

    void init();
    void update();
    size_t size();
    Planet* getPlanet(int index); //returns planet at "index", null if it does not exist
    template<typename T>
    void processPlanets(T func, int layer = -1)
    {
        /**
          *   \brief Pass a function that is then run on each planet in "layer". Return true if you wish to terminate early
          *
          *
          *   \param func: A "Callable", which is anything with a () operator. T = (Planet&) -> void or bool
          *   \param layer: The layer you want, with 0 being closest to the outer edge of the solar system. -1 if you wish to run on all planets.
          *   \return An int
          *
          **/
        int i = 0;
        bool layerOutOfRange = layer < 0 || layer >= layers.size(); //whether "layer" is out of range
        auto start = layerOutOfRange ? 0 : layers[layer]; //if out of range, start at the beginning, otherwise start at the right layer
        auto end = layerOutOfRange || layer == layers.size() - 1 ? planets.size() : layers[layer + 1]; //if out of range or the chosen layer is the last one, go to the end, otherwise go up to the next layer
        for (auto it = start; it != end; ++it)
        {
            if constexpr (!std::is_same<decltype(func(std::declval<Planet&>())),bool>::value) //if the function doesn't return a bool, keep going until we've processed every planet
            {
                func(*planets[it]);
            }
            else //if the function returns bool and is true, we stop running
            {
                auto val = func(*planets[it]);
                if (val)
                {
                    return;
                }
            }
        }
    }
    template<typename T, typename I>
    I foldLeftPlanets(T func, I initial) //
    {
        //T = (Planet&, I) -> I
        /**
        * \brief Given a function and an initial value, run it on each planet and return a value accordingly
        * \param func: A function that processes a planet and a value and returns the next value to be processed, (Planet&, I) -> I
        * \param initial: A value of any type
        **/
        I value = initial;
        processPlanets([&value,&func](Planet& planet){
                       value = func(planet, value);
                       });
        return value;
    }
};

#endif //STARSYSTEM_H_INCLUDED
