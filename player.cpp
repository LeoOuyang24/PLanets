#include "SDLhelper.h"
#include "geometry.h"

#include "player.h"
#include "game.h"
#include "projectile.h"

PlayerControlsComponent::PlayerControlsComponent(Entity& player) : Component(player), ComponentContainer<PlayerControlsComponent>(player)
{

}

bool PlayerControlsComponent::getMovedLastFrame()
{
    return movedLastFrame;
}

void PlayerControlsComponent::update()
{
    PlayerMoveComponent* move = entity->getComponent<PlayerMoveComponent>();
    GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>();
    glm::vec2 mousePos = ViewPort::toWorld(pairtoVec(MouseManager::getMousePos()));

    if (move)
    {
        if (MouseManager::getJustClicked() == SDL_BUTTON_LEFT)
        {
            Entity* bullet =new Entity();
            bullet->addComponent(*(new RectRenderComponent(*bullet,{1,1,1,1})));
            bullet->addComponent(*(new GravityForcesComponent(*bullet)));
            bullet->addComponent(*(new RectComponent(glm::vec4(0,0,10,10),*bullet)));
            bullet->addComponent(*(new ProjectileDeathComponent(*bullet)));
            glm::vec2 shootForce = 5.0f*betterNormalize(mousePos - move->getCenter());
            bullet->getComponent<GravityForcesComponent>()->addForce(shootForce);
            Game::getManager().addEntity(*bullet,move->getCenter().x,move->getCenter().y);
        }
        PlayerHealthComponent* health = entity->getComponent<PlayerHealthComponent>();
        bool AIsPressed = KeyManager::isPressed(SDLK_a);
        bool DIsPressed = KeyManager::isPressed(SDLK_d);
        if (KeyManager::getJustPressed() == SDLK_d)
        {
            move->setFacing(FORWARD);
        }
        else if (KeyManager::getJustPressed() == SDLK_a)
        {
            move->setFacing(BACKWARD);
        }
        if (!health->getFallingBack())
        {
            bool moveLeftOrRight = (AIsPressed || DIsPressed);
            if (moveLeftOrRight)
            {
                move->setSpeed(KeyManager::isPressed(SDLK_LCTRL) ? PlayerMoveComponent::PLAYER_SPRINT_SPEED : PlayerMoveComponent::PLAYER_SPEED);
                move->accel(DIsPressed); //move clockwise if D is pressed, otherwise move counter clockwise //sprint if sprinting
                if (EntityAnimationComponent* comp = entity->getComponent<EntityAnimationComponent>())
                {
                    comp->setAnimation({{0,0,1,0.5},8,1,16});
                }
            }
            else
            {
                if (EntityAnimationComponent* comp = entity->getComponent<EntityAnimationComponent>())
                {
                    comp->setAnimation({{0,0,.125,0.5}});
                }
            }

            if (KeyManager::getJustPressed() == SDLK_SPACE)
            {
                if (forces && move->getOnGround())
                {
                    glm::vec2 jumpVec = glm::normalize(move->getCenter() - move->getStandingOn()->center);
                    if ((moveLeftOrRight))
                    {
                        glm::vec2 forwardVec = (1.0f - 2*move->getFacing())*glm::vec2(-jumpVec.y,jumpVec.x); //calculate horizontal vector, multiply by -1 if moving clockwise
                        //forwardVec = {-cos(move->getTilt()), -sin(move->getTilt())};
                        //jumpVec = jumpVec + (move->getMovedAmount()/move->getBaseSpeed())*forwardVec;
                    }
                    forces->addForce(jumpVec); //+ .5f*moveAmount*glm::normalize(glm::vec2(planetToPlayerVec.y,-planetToPlayerVec.x)));
                    //onGround = false;
                }
            }
            if (KeyManager::isPressed(SDLK_LSHIFT) || KeyManager::isPressed(SDLK_RSHIFT))
            {
                if (forces && fuel > 0)
                {
                    float tilt = atan2(mousePos.y - move->getCenter().y, mousePos.x - move->getCenter().x);
                    move->setTilt(tilt);
                    forces->addForce(0.1f*ForceVector(cos(tilt),sin(tilt)));
                    fuel = std::max(0,fuel - 1);
                }
            }
            else
            {
                fuel = std::min(MAX_FUEL,fuel + 1);
            }
        }

        //move->moveCenter(move->moveOnCircle(move->getSpeed()));

        PolyRender::requestLine(glm::vec4(move->getCenter(),mousePos),glm::vec4(1,0,0,1),1,1);
        PolyRender::requestCircle(glm::vec4(1,0,0,1),mousePos,10,false,1);

        //setMovedLastFrame(moveLeftOrRight);
    }
}

void PlayerControlsComponent::setMovedLastFrame(bool moved_)
{
    movedLastFrame = moved_;
}

PlayerMoveComponent::PlayerMoveComponent(Entity& player) : MoveOnPlanetComponent({0,0,PLAYER_DIMEN,PLAYER_DIMEN},{PLAYER_DIMEN,PLAYER_ACCEL,PLAYER_DECEL,PLAYER_IN_AIR_ACCEL},player), ComponentContainer<PlayerMoveComponent>(player)
{

}

void PlayerMoveComponent::setStandingOn(Planet* planet)
{
    float oldTilt = tilt;
    MoveOnPlanetComponent::setStandingOn(planet);
    if (planet)
    {
        if (abs(oldTilt - tilt) >= M_PI)
        {
            velocity *= -1;

        }
    }
}


void PlayerMoveComponent::update()
{
    //moveCenter(moveOnCircle(velocity));
    //setFacing(velocity > 0 ? FORWARD : BACKWARD);
    MoveOnPlanetComponent::update();
}

PlayerHealthComponent::PlayerHealthComponent(Entity& player) : BaseHealthComponent(1000,PlayerHealthComponent::PLAYER_MAX_HEALTH,PlayerHealthComponent::PLAYER_MAX_HEALTH,player), ComponentContainer<PlayerHealthComponent>(player)
{

}

void PlayerHealthComponent::takeDamage(Entity& source, int damage)
{
    if (damage > 0 && !isInvuln())
    {
        if (ForcesComponent* forces = entity->getComponent<ForcesComponent>())
        if (RectComponent* sourcePos = source.getComponent<RectComponent>())
        if (MoveOnPlanetComponent* ourPos = entity->getComponent<MoveOnPlanetComponent>())
        {
            glm::vec2 hurtVec = betterNormalize(ourPos->getCenter() - sourcePos->getCenter() );
            if (!ourPos->getOnGround())
            {
                fallingBack = true;
                forces->addForce(hurtVec);
            }
            else
            {
                fallingBack = true;
                ourPos->accelMult(-1);
            }
            //hurtVec *= .1f;
        }
        BaseHealthComponent::addHealth(-damage);
    }
}

void PlayerHealthComponent::update()
{
    if (fallingBack)
    {
        if (EntityAnimationComponent* anime = entity->getComponent<EntityAnimationComponent>())
        {
            anime->setAnimation({{0,0.5,0.125,0.5},1,1});
        }
        if (!isInvuln())
        {
            fallingBack = false;
        }
        else
        {
            if (MoveOnPlanetComponent* move = entity->getComponent<MoveOnPlanetComponent>())
            if (move->getOnGround() && (invuln.getTimePassed() >= invulnTime*.3))
            {
                fallingBack = false;
            }
        }
    }
}

bool PlayerHealthComponent::getFallingBack()
{
    return fallingBack;
}


void Player::update(StarSystem& system, RenderCamera& camera)
{
    glm::vec2 center = glm::vec2(rect.x + rect.z/2, rect.y + rect.a/2);
    if (standingOn)
    {
        glm::vec2 planetToPlayerVec = {standingOn->center.x - center.x,standingOn->center.y - center.y};
        angle = atan2(planetToPlayerVec.y,planetToPlayerVec.x);
        glm::vec2 moveVec = glm::vec2(0);
        float moveAmount = 0;
        onGround = pointDistance(center,standingOn->center) <= standingOn->radius;

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


        standingOn = glm::distance(center,standingOn->center) <= standingOn->getGravityRadius() ? standingOn : 0;
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
                              if (pointDistance(center,planet.center) <= planet.radius)
                              {
                                  standingOn = &planet;
                                  onGround = true;
                              }
                              if (pointDistance(center,planet.center) <= planet.getGravityRadius())
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

Entity* Player::createPlayer(StarSystem& system)
{
    Entity* player = new Entity();
    player->addComponent(*(new PlayerControlsComponent(*player)));
    player->addComponent(*(new PlayerMoveComponent(*player)));
    //player->addComponent(*(new MoveOnPlanetComponent(*system.getPlanet(0),{20,30},1,*player)));
    player->addComponent(*(new GravityForcesComponent(*player)));
    player->addComponent(*(new PlayerHealthComponent(*player)));
    //player->addComponent(*(new RectRenderComponent(*player,glm::vec4(1,0,0,1))));
    Sprite* sprite = new Sprite("sprites/guy.png");
    //BaseAnimation anime = {2,8,1, {0,0,1,1}};
    player->addComponent(*(new EntityAnimationComponent(*player, *sprite,1)));
    return player;
}
