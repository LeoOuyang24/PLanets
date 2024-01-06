

#include "starSystem.h"
#include "game.h"
#include "entity.h"
#include "player.h"

int StarSystem::getPlanetZGivenLayer(int layer)
{
    return layer*LAYER_SPACE*-1 + PLANET_Z;
}

int StarSystem::getZGivenLayer(int layer)
{
    return layer*LAYER_SPACE*-1 + GAME_Z;
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
    planet.layer = layer;
    if (layer < layers.size())
    {
        planets.insert(planets.begin() + layers[layer],std::shared_ptr<Planet>(&planet));
    }
    else
    {
        layers.push_back(planets.size()); //add the new layer
        planets.insert(planets.begin() + planets.size(), std::shared_ptr<Planet>(&planet)); //insert into next layer
    }
}

void StarSystem::init()
{
    planets.clear();
    layers.clear();
    int numOfLayers = rand()%3 + 2;
    for (int i =0; i<numOfLayers; ++i)
    {
        int layerSize = 3;//(i + 1)*3;//rand()%(MAX_PLANETS - MIN_PLANETS + 1) + MIN_PLANETS;
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
    processPlanets([](PlanetPtr& planet){
                   int playerLayer = Game::getPlayer().getComponent<MoveOnPlanetComponent>()->getLayer();
                   planet->render();
                   if (planet->layer == playerLayer)
                   {
                       planet->renderGravityField();
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

glm::vec2 LaunchComponent::getLandingSpotOnPlanet(PlanetIndex planet, float angle)
{
    if (Planet* planetPtr = Game::getSolar().getPlanet(planet))
    {
        return planetPtr->getPlanetSurfacePoint(angle,planetPtr->center);
    }
    return glm::vec2(0,0);
}

LaunchComponent::LaunchComponent(PlanetIndex planet, const glm::vec2& destination, Entity& entity) : Component(entity),
                                                                                                ComponentContainer<LaunchComponent>(entity),
                                                                                                destinationPlanet(planet),
                                                                                                destinationPoint(destination)
{
    launchSequence.setup(*(new SequenceUnit(1,0,true,[&entity,this](int time){
                                            if (MoveOnPlanetComponent* move = entity.getComponent<MoveOnPlanetComponent>())
                                            {
                                                if (RenderCamera* camera = ViewPort::currentCamera)
                                                if (!vecContains(move->getRect(),camera->getRect()))
                                                {
                                                    move->setLayer(move->getLayer() + 1);
                                                    move->setTilt(getPointAngle(destinationPoint,move->getCenter()));
                                                    return true;
                                                }
                                                else
                                                {
                                                    float tilt = move->getTilt();
                                                    move->setMoveVec(std::min(pow(time/100.0f,2.0f),10.0f)*glm::vec2(cos(tilt),sin(tilt)));
                                                    //move->setCenter(move->getCenter() + std::min(pow(time/100.0f,2.0f),10.0f)*glm::vec2(cos(tilt),sin(tilt)));
                                                   // std::cout << std::min(pow(2.0f,time/1000.0f),3.0f) << "\n";
                                                    //code below is to render a rectangle showing the rocket's launch "trail"
                                                    //glm::vec2 debrisCenter = move->getCenter() - (move->getRect().z/2 + time/2)*glm::vec2(cos(tilt),sin(tilt));
                                                    //PolyRender::requestRect(glm::vec4(debrisCenter - glm::vec2(time/2,move->getRect().a/2),time,move->getRect().a),
                                                    //                        {1,0,1,1},true,tilt,StarSystem::getPlanetZGivenLayer(move->getLayer()));
                                                    //std::cout << tilt << " " << getPointAngle(move->getCenter(),entity.getComponent<MoveOnPlanetComponent>()->getStandingOn()->center) << "\n";
                                                }
                                            }
                                            return false;
                                            })),
                            *(new SequenceUnit(1,0,true,[&entity,this](int){
                                if (MoveOnPlanetComponent* move = entity.getComponent<MoveOnPlanetComponent>())
                                {
                                    PolyRender::requestCircle({1,0,0,1},destinationPoint,10,true,StarSystem::getPlanetZGivenLayer(move->getLayer()));
                                    if (pointInVec(move->getRect(),destinationPoint))
                                    {
                                        return true;
                                    }
                                    else
                                    {
                                        float tilt = move->getTilt();
                                        move->setCenter(move->getCenter() + 1.0f*glm::vec2(cos(tilt),sin(tilt)));
                                        return false;
                                        //std::cout << tilt << " " << getPointAngle(move->getCenter(),entity.getComponent<MoveOnPlanetComponent>()->getStandingOn()->center) << "\n";
                                    }
                                }
                                return false;
                              })));
}

LaunchComponent::LaunchComponent(PlanetIndex planet, float angle, Entity& entity) : LaunchComponent(planet,
                                                                                                    LaunchComponent::getLandingSpotOnPlanet(planet,angle)
                                                                                                    ,entity)
{

}

void LaunchComponent::collide(Entity& other)
{
    if (auto move = other.getComponent<PlayerMoveComponent>())
    {
        Planet* planet = Game::getSolar().getPlanet(destinationPlanet);
        if (planet)
        {
            //move->setLayer(planet->layer);
            //move->setStandingOn(planet);
            SequenceManager::request(launchSequence);
        }

    }
}
