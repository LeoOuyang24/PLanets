#include "SDLhelper.h"
#include "geometry.h"

#include "player.h"

void Player::update()
{
    if (standingOn)
    {
        glm::vec2 center = glm::vec2(rect.x + rect.z/2, rect.y + rect.a/2);
        glm::vec2 planetToPlayerVec = {standingOn->center.x - center.x,standingOn->center.y - center.y};
        angle = atan2(planetToPlayerVec.y,planetToPlayerVec.x);
        glm::vec2 moveVec = glm::vec2(0);
        onGround = FLOAT_COMPARE(pointDistance(center,standingOn->center),standingOn->radius,3,<);

        float moveAmount = 0;
        if (KeyManager::isPressed(SDLK_LEFT))
        {
            moveAmount += -1*speed;
        }
        if (KeyManager::isPressed(SDLK_RIGHT))
        {
            moveAmount += speed;
        }

        float moveAngle = angle + moveAmount/(standingOn->radius);

        if (onGround)
        {
            forces = glm::vec2(0);
            if (KeyManager::getJustPressed() == SDLK_SPACE)
            {
                forces += -0.75f*glm::normalize(planetToPlayerVec);
            }
        }
        else
        {
            forces += .01f/standingOn->radius*planetToPlayerVec;
        }
        if (forces.length() > 2)
        {
            forces = 2.f*glm::normalize(forces);
        }
        forces *= .999;
        PolyRender::requestLine({center,center + 100.0f*forces},glm::vec4(1,0,0,1),1,1,0);
        float dist = std::max((float)standingOn->radius,glm::distance(glm::vec2(rect.x + rect.z/2,rect.y +rect.a/2),standingOn->center));
        rect.x = standingOn->center.x - cos(moveAngle)*dist - rect.z/2;
        rect.y = standingOn->center.y - sin(moveAngle)*dist - rect.a/2;
        rect.x += forces.x;
        rect.y += forces.y;

    }
    PolyRender::requestRect(rect,{1,0,0,1},true,angle,1);
}
