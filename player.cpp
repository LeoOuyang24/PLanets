#include "SDLhelper.h"
#include "geometry.h"

#include "player.h"
#include "game.h"
#include "projectile.h"

PlayerForcesComponent::PlayerForcesComponent(Entity& player) : GravityForcesComponent(player), ComponentContainer<PlayerForcesComponent>(player)
{

}

float PlayerForcesComponent::getFriction()
{
        return GravityForcesComponent::getFriction();
}

void PlayerForcesComponent::update()
{
    //Force oldForce =

    if (entity)
    if (PlayerMoveComponent* move = entity->getComponent<PlayerMoveComponent>())
    if (move->getLatchedTo())
    {
        for_each(forces.begin(), forces.end(), [this](const std::pair<ForceSource,Force>& f){ //if we are latched to a planet, clear all forces except our own velocity
                 if (f.first != SELF)
                 {
                     applyFriction(0,f.first);
                 }
                 });
    }
    GravityForcesComponent::update();
}

PlayerControlsComponent::PlayerControlsComponent(Entity& player) : Component(player), ComponentContainer<PlayerControlsComponent>(player)
{

}

bool PlayerControlsComponent::getMovedLastFrame()
{
    return movedLastFrame;
}

void PlayerControlsComponent::update()
{
    if (entity)
    {
    PlayerMoveComponent* move = entity->getComponent<PlayerMoveComponent>();
    PlayerForcesComponent* forces = entity->getComponent<PlayerForcesComponent>();
    glm::vec2 mousePos = ViewPort::toWorld(pairtoVec(MouseManager::getMousePos()));
    //PolyRender::requestCircle({1,0,0,1},mousePos,10,1,Game::getSolar().getZGivenLayer(move->getLayer()));
    if (move)
    {
        if (MouseManager::getJustClicked() == SDL_BUTTON_LEFT)
        {
            /*Entity* bullet =new Entity();
            bullet->addComponent(*(new RectRenderComponent(*bullet,{1,1,1,1})));
            bullet->addComponent(*(new GravityForcesComponent(*bullet)));
            bullet->addComponent(*(new RectComponent(glm::vec4(0,0,10,10),*bullet)));
            bullet->addComponent(*(new ProjectileDeathComponent(*bullet)));
            glm::vec2 shootForce = 5.0f*betterNormalize(mousePos - move->getCenter());
            bullet->getComponent<GravityForcesComponent>()->addForce(shootForce);
            Game::getManager().addEntity(*bullet,move->getCenter().x,move->getCenter().y);*/
        }
        else if (forces)
        {
            if (MouseManager::isPressed(SDL_BUTTON_RIGHT))
            {
                if (!move->getOnGround())
                {
                    Game::getSolar().processPlanets([mousePos, move](std::shared_ptr<Planet>& planet){
                                                    //PolyRender::requestCircle({1,0,0,1},planet->center,10,1,Game::getSolar().getZGivenLayer(planet->layer));
                                                    if (pointDistance(mousePos,planet->center) <= planet->radius)
                                                    {
                                                        move->setLatchedTo(planet);
                                                        return true;
                                                    }
                                                    else
                                                    {
                                                        //std::cout << pointDistance(mousePos,planet->center) << " " <<  planet->radius << "\n";
                                                    }
                                                    return false;
                                                    },move->getLayer());
                    //forces->setRotating(true);
                }
            }
            else
            {
                //forces->setRotating(false);
                move->setLatchedTo(PlanetPtr());
            }
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
            //std::cout << "VEL: " << move->getVelocity() << "\n";
            bool moveLeftOrRight = (AIsPressed || DIsPressed);

            if (moveLeftOrRight)
            {
                move->setSpeed(PlayerMoveComponent::PLAYER_SPEED);
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
                    if ((moveLeftOrRight) && KeyManager::isPressed(SDLK_LCTRL))
                    {
                        glm::vec2 forwardVec = (1.0f - 2*move->getFacing())*glm::vec2(-jumpVec.y,jumpVec.x); //calculate horizontal vector, multiply by -1 if moving clockwise
                        //forwardVec = {-cos(move->getTilt()), -sin(move->getTilt())};
                        jumpVec = 0.5f*jumpVec + forwardVec;
                    }
                    //std::cout << "JUMP: " << glm::length(0.75f*jumpVec) << "\n";
                    forces->addForce(20.0f*jumpVec,JUMP); //+ .5f*moveAmount*glm::normalize(glm::vec2(planetToPlayerVec.y,-planetToPlayerVec.x)));
                     //   std::cout << glm::length(0.75f*jumpVec) << " " << glm::length(forces->getForce(JUMP))<< "\n";

                    //onGround = false;
                }
            }
            if (KeyManager::isPressed(SDLK_LSHIFT) || KeyManager::isPressed(SDLK_RSHIFT))
            {
                if (forces && fuel > 0)
                {
                    float tilt = atan2(mousePos.y - move->getCenter().y, mousePos.x - move->getCenter().x);
                    move->setTilt(tilt);
                    forces->addForce(1.0f*ForceVector(cos(tilt),sin(tilt)), JUMP);
                    fuel = std::max(0,fuel - 1);
                }
            }
            else
            {
                fuel = std::min(MAX_FUEL,fuel + 1);
            }
            if (KeyManager::getJustPressed() == SDLK_LCTRL && move->getOnGround())
            {
                forces->applyFrictionAll(1.3f);
            }
        }

        //move->moveCenter(move->moveOnCircle(move->getSpeed()));

        //PolyRender::requestLine(glm::vec4(move->getCenter(),mousePos),glm::vec4(1,0,0,1),Game::getCurrentZ(),1);
        PolyRender::requestCircle(glm::vec4(1,0,0,1),mousePos,10,false,Game::getCurrentZ());
        if (move && move->getStandingOn())
        {
            PolyRender::requestLine(glm::vec4(move->getCenter(),move->getStandingOn()->center),{1,0,0,1},Game::getCurrentZ());
        }

        //setMovedLastFrame(moveLeftOrRight);
    }
    }
}

PlayerMoveComponent::PlayerMoveComponent(Entity& player) : MoveOnPlanetComponent({0,0,PLAYER_DIMEN,PLAYER_DIMEN},{PLAYER_DIMEN,PLAYER_ACCEL,PLAYER_DECEL,PLAYER_IN_AIR_ACCEL},player), ComponentContainer<PlayerMoveComponent>(player)
{

}

void PlayerMoveComponent::setLatchedTo(const std::shared_ptr<Planet>& planet)
{
    if (latchedTo.lock().get() != planet.get())
    {
        latchedTo = planet;
        if (planet.get())
        {
            setStandingOn(planet.get());
            latchedVelocity = getVelocity();
        }
    }
}

DeltaTime& PlayerMoveComponent::getWarping()
{
    return warping;
}

Planet* PlayerMoveComponent::getLatchedTo()
{
    return latchedTo.lock().get();
}

void PlayerMoveComponent::setStandingOn(Planet* planet)
{
    float oldTilt = tilt;
    MoveOnPlanetComponent::setStandingOn(planet);
    if (planet)
    {
        if (abs(oldTilt - tilt) >= M_PI)
        {
            //velocity *= -1;
        }
    }
}


void PlayerMoveComponent::update()
{
    //moveCenter(moveOnCircle(velocity));
    //setFacing(velocity > 0 ? FORWARD : BACKWARD);
    if (getOnGround())
    {
        setLatchedTo(PlanetPtr());
    }
    else if (auto latched = latchedTo.lock().get())
    {
        PolyRender::requestLine(glm::vec4(getCenter(),latched->center),{0,1,1,1},StarSystem::getPlanetZGivenLayer(getLayer()));
        setVelocity(latchedVelocity);
    }
    MoveOnPlanetComponent::update();
    //std::cout << latchedVelocity << " " << getVelocity() << "\n";

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


PlayerAnimationComponent::PlayerAnimationComponent(Entity& entity, Sprite& sprite) : EntityAnimationComponent(entity, sprite),
                                                                                     weightlessOutline({
                                                                                                       {
                                                                                                       templateShader(readFile("./shaders/spriteShader.h").first,true,{"vec4 shade"},{"vec4 tint"},{"tint = shade"})
                                                                                                       ,GL_VERTEX_SHADER,false
                                                                                                       },
                                                                                                       LoadShaderInfo{"shaders/paintShader.h",GL_FRAGMENT_SHADER}})
{

}

void PlayerAnimationComponent::update()
{
    //EntityAnimationComponent::update();
    if (GravityForcesComponent* forces = entity->getComponent<GravityForcesComponent>())
    if (MoveOnPlanetComponent* rect = entity->getComponent<MoveOnPlanetComponent>())
    if (BaseHealthComponent* health = entity->getComponent<BaseHealthComponent>())
    if (forces->getWeightless())
    {
        BaseAnimationComponent::request(weightlessOutline,rect->getRect(),StarSystem::getPlanetZGivenLayer(rect->getLayer()),
                                        BaseAnimation::getFrameFromStart(start,anime),
                                        rect->getTiltOnPlanet(),
                                        rect->getFacing() == FORWARD ? NONE : MIRROR,//if we are facing backwards, mirror the sprite (this means all sprites by default need to be facing to the right
                                        health && health->isInvuln() ? 1.0f : 0.0f,
                                        glm::vec4(0,1,0,1)
                                        );
    }
    else
    {
        EntityAnimationComponent::update();
    }

}

Entity* Player::createPlayer(StarSystem& system)
{
    Entity* player = new Entity();
    //player->addComponent(*(new MoveOnPlanetComponent(*system.getPlanet(0),{20,30},1,*player)));
    player->addComponent(*(new PlayerForcesComponent(*player)));
    player->addComponent(*(new PlayerHealthComponent(*player)));
        player->addComponent(*(new PlayerControlsComponent(*player)));
    player->addComponent(*(new PlayerMoveComponent(*player)));

    //player->addComponent(*(new RectRenderComponent(*player,glm::vec4(1,0,0,1))));
    Sprite* sprite = new Sprite("sprites/guy.png");
    //BaseAnimation anime = {2,8,1, {0,0,1,1}};
    player->addComponent(*(new PlayerAnimationComponent(*player, *sprite)));
    return player;
}
