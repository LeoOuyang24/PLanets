#include "SDLhelper.h"
#include "geometry.h"

#include "player.h"

void Player::update(StarSystem& system)
{
    glm::vec2 center = glm::vec2(rect.x + rect.z/2, rect.y + rect.a/2);
    if (standingOn)
    {
        glm::vec2 planetToPlayerVec = {standingOn->center.x - center.x,standingOn->center.y - center.y};
        angle = atan2(planetToPlayerVec.y,planetToPlayerVec.x);
        glm::vec2 moveVec = glm::vec2(0);
        float moveAmount = 0;
        onGround = FLOAT_COMPARE(pointDistance(center,standingOn->center),standingOn->radius,3,<);

        if (KeyManager::isPressed(SDLK_LEFT) || KeyManager::isPressed(SDLK_RIGHT))
        {
            //if (onGround || !airMoved)
            {
                moveAmount += (1 - KeyManager::isPressed(SDLK_LEFT) * 2 )*speed*(.3*onGround + .7);
            }
            //airMoved = true;
        }
        if (onGround)
        {
            airMoved = false;
            if (KeyManager::getJustPressed() == SDLK_SPACE)
            {
                forces += -1.f*glm::normalize(planetToPlayerVec) + .5f*moveAmount*glm::normalize(glm::vec2(planetToPlayerVec.y,-planetToPlayerVec.x));
                onGround = false;
            }
        }
        float moveAngle = angle + moveAmount/(standingOn->radius);
        //forces += moveAmount*glm::normalize(glm::vec2(planetToPlayerVec.y,-planetToPlayerVec.x));
        float dist = std::max((float)standingOn->radius,glm::distance(glm::vec2(rect.x + rect.z/2,rect.y +rect.a/2),standingOn->center));
        rect.x = standingOn->center.x - cos(moveAngle)*dist - rect.z/2;
        rect.y = standingOn->center.y - sin(moveAngle)*dist - rect.a/2;


        standingOn = FLOAT_COMPARE(glm::distance(center,standingOn->center),standingOn->getGravityRadius(),3,<) ? standingOn : 0;
    }
    if (KeyManager::isPressed(SDLK_LSHIFT) && fuel)
    {
        forces *= 1.1f;
        fuel = std::max(0,fuel - 5);
        fuelRecharge.set();
    }
    if (fuelRecharge.getTimePassed() >= 2000)
    {
        fuel = std::min(100, fuel + 1);
    }
    if (forces.length() > 3)
    {
        forces = 3.f*glm::normalize(forces);
    }
    PolyRender::requestLine({center,center + 100.0f*forces},glm::vec4(1,0,0,1),1,1,0);

    rect.x += forces.x;
    rect.y += forces.y;
    if (!onGround)
    {
        system.processPlanets([this,center](Planet& planet){
                              if (FLOAT_COMPARE(pointDistance(center,planet.center),planet.radius,3,<))
                              {
                                  standingOn = &planet;
                                  onGround = true;
                              }
                              if (FLOAT_COMPARE(pointDistance(center,planet.center),planet.getGravityRadius(),3,<))
                              {
                                  glm::vec2 newForce = (planet.radius/100.0f)/(pow(planet.center.x - center.x,2.0f) + pow(planet.center.y - center.y,2.0f))*(planet.center - center);
                                  PolyRender::requestLine(glm::vec4(center,center + 10000.0f*newForce),{0,1,(&planet == standingOn),1},1,1,0);
                                  forces += newForce;
                              }

                              });
        //forces += .01f/standingOn->radius*planetToPlayerVec;
       forces *= .999;
    }
    else
    {
        forces *= 0;
    }
    PolyRender::requestRect(rect,{1,0,0,1},true,angle,1);
}
