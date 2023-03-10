#include "SDLhelper.h"
#include "geometry.h"

#include "player.h"

void Player::update(StarSystem& system, RenderCamera& camera)
{
    glm::vec2 center = glm::vec2(rect.x + rect.z/2, rect.y + rect.a/2);
    if (standingOn)
    {
        glm::vec2 planetToPlayerVec = {standingOn->center.x - center.x,standingOn->center.y - center.y};
        angle = atan2(planetToPlayerVec.y,planetToPlayerVec.x);
        glm::vec2 moveVec = glm::vec2(0);
        float moveAmount = 0;
        onGround = FLOAT_COMPARE(pointDistance(center,standingOn->center),standingOn->radius,3,<);

        if (KeyManager::isPressed(SDLK_a) || KeyManager::isPressed(SDLK_d))
        {
            //if (onGround || !airMoved)
            {
                moveAmount += (1 - KeyManager::isPressed(SDLK_a) * 2 )*speed*(.3*onGround + .7);
            }
            if (onGround)
            {
                float baseWidth = 10;
                float baseHeight = 2;
                int amount= 5;
                int dimen = 5;
                for (int i = 0; i < amount;i ++)
                {
                    int duration = 1000;
                    int wait = i*100;
                    int width = baseWidth + (i%2*2 - 1)*baseWidth/2;
                    int height = baseHeight + (1 - i%2*2)*baseHeight/2;
                    int ticks = SDL_GetTicks()%(wait + duration);
                    if (ticks < duration)
                    {
                        float y = pow(ticks-duration/4,2)/(float)(pow(duration/2,2))*baseHeight;
                        glm::vec2 origin = glm::vec2(rect.x + rect.z/(amount)*i,rect.y + rect.a - (dimen));
                        glm::vec2 point = rotatePoint({origin.x + (i%2*2 - 1)*ticks/(float)duration*width,origin.y  + y},center,angle-M_PI/2);
                        PolyRender::requestRect(glm::vec4(point,dimen,dimen),glm::vec4(0,1,0,1),true,0,2);
                    }
                }

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
    glm::vec2 mousePos = camera.toWorld(pairtoVec(MouseManager::getMousePos()));
    if (KeyManager::isPressed(SDLK_LSHIFT) && fuel)
    {
        forces += .1f*glm::normalize(mousePos - center);
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
    PolyRender::requestLine({center,center + 100.0f*forces},glm::vec4(1,0,0,1),1,1);

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
                                  PolyRender::requestLine(glm::vec4(center,center + 10000.0f*newForce),{0,1,(&planet == standingOn),1},1,1);
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
    cursorUI.draw(center,mousePos);
}

CursorUI::CursorUI(std::string vertex, std::string fragment) //line,z,non-transluscent color
{
    program.init(vertex,fragment);
    glBindVertexArray(program.VAO);

    glBindVertexArray(0);
    ViewPort::linkUniformBuffer(program.program);
}

void CursorUI::draw(const glm::vec2& origin, const glm::vec2& mousePos)
{
    glm::vec4 points = glm::vec4(origin,mousePos);
    std::vector<float> nums;
    nums.push_back(origin.x);
    nums.push_back(origin.y);

    /*nums.push_back(mousePos.x);
    nums.push_back(origin.y);

    nums.push_back(origin.x);
    nums.push_back(mousePos.y);*/

    nums.push_back(mousePos.x);
    nums.push_back(mousePos.y);


    glBindVertexArray(program.VAO);
    glBindBuffer(GL_ARRAY_BUFFER,program.VBO);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*nums.size(),&nums[0],GL_DYNAMIC_DRAW);
    glUseProgram(program.program);

    glUniform2f(glGetUniformLocation(program.program,"screenDimen"),ViewPort::screenWidth,ViewPort::screenHeight);

    glLineWidth(3.0);

    glDrawArrays(GL_LINES,0,nums.size()/2);
    glLineWidth(1.0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    PolyRender::requestCircle({1,0,0,1},mousePos,10,false,1);
}
