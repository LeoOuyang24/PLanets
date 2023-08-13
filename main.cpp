#include <iostream>
#include <time.h>
#include <SDL.h>

#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"

#include "planet.h"
#include "player.h"
#include "starSystem.h"
#include "shaders.h"
#include "follower.h"
#include "critter.h"
#include "AI.h"
#include "game.h"
#include "enemy.h"

int main(int args, char* argsc[])
{
    //delete ptr;
    const int screenWidth = 640;
    const int screenHeight = 640;

    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

    GLContext::init(screenWidth,screenHeight);

    glEnable(GL_MULTISAMPLE);

    SDL_StopTextInput();
    ViewPort::init(screenWidth,screenHeight);

    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);
    SDL_Event e;
    bool quit = false;
    glClearColor(0,0,0,1);
    bool eventsEmpty = true;
        //std::cout << tree.count() << std::endl;

    PlanetSprites.init();


    //Planet earth = {{500,screenHeight},200};
    //earth.sprite = 0;//"./planets/grassPlanet.png";
    //Player player;
    //player.standingOn = solar.getPlanet(0);
    //if (player.standingOn)
    //player.rect = {player.standingOn->center.x,player.standingOn->center.y - player.standingOn->radius,20,20};

    Game::init();
//REFACTOR: Make it so functions that need solar system just get it from Game rather than getting it passed in.



    Sprite chomper ("sprites/chomperSheet.png");
    Entity* rabbit = new Entity();
    rabbit->addComponent(*(new MoveOnPlanetComponent(*Game::getSolar().getPlanet(0),{30,30},{},*rabbit)));
    //rabbit.addComponent(*(new RabbitAI(rabbit)));
    rabbit->addComponent(*(new EntityAnimationComponent(*rabbit,chomper,1)));
    //rabbit.addComponent(*(new RectRenderComponent(rabbit,{1,1,1,1})));
    rabbit->addComponent(*(new GravityForcesComponent(*rabbit)));
    rabbit->addComponent(*(new AIComponent(*rabbit, AIStateBase::PATROL,
                                          *(new AIState(AIStateBase::PATROL,HopAIFunc(0.5))),
                                          *(new AIState(AIStateBase::IDLE,[rabbit](int frames, Entity&){
                                                        if (EntityAnimationComponent* anime = rabbit->getComponent<EntityAnimationComponent>())
                                                        {
                                                            anime->setAnimation({BaseAnimation::normalizePixels({0,0,0.2,246},*anime->getSpriteSheet()),1,1});
                                                        }

                                                        return frames > 100 ? AIStateBase::PATROL : AIStateBase::NOT_DONE;})),
                                          *(new AIState(AIStateBase::ATTACK,ChomperAttackFunc({}))),
                                          *(new AIState(AIStateBase::WIND_UP, ChomperWindUpFunc({})))
                                          )));
    rabbit->addComponent(*(new EnemyComponent(1,*rabbit)));

    Sprite spitter_sprite ("sprites/spitter_idle.png");
    Entity* spitter = new Entity();
    spitter->addComponent(*(new MoveOnPlanetComponent(*Game::getSolar().getPlanet(0),{60,60},{},*spitter)));
    //rabbit.addComponent(*(new RabbitAI(rabbit)));
    spitter->addComponent(*(new EntityAnimationComponent(*spitter,spitter_sprite,1,{BaseAnimation::normalizePixels({0,0,1,1},spitter_sprite),4,2,8})));
    //rabbit.addComponent(*(new RectRenderComponent(rabbit,{1,1,1,1})));

    Entity* solarian = new Entity();
    solarian->addComponent(*(new BasicMoveComponent(glm::vec4(100,100 ,30,30),*solarian)));
    solarian->addComponent(*(new AIComponent(*solarian,AIStateBase::IDLE,*(new IdleState(1000,AIStateBase::PATROL)),*(new AerialWanderFunc()))));
    solarian->addComponent(*(new SolarianRenderComponent(*solarian)));
    //BaseAnimationComponent comp(rabbit,anime);

    Game::getManager().addEntity(*rabbit);
    Game::getManager().addEntity(*solarian);
    //Game::getManager().addEntity(*spitter);

    Sprite background;
    background.init("sprites/blueplanet.png");
    RenderCamera camera;
    camera.init(screenWidth,screenHeight);
    Sprite bunny("planets/red.png");

    ViewPort::currentCamera = &camera;
    //BackgroundProgram backgroundProgram;


    //Follower apple({30,30,30,30},&player,&appleSprite);


    //RenderProgram test()

    //glm::mat4 proj = glm::ortho(0.0f,screenWidth*5.0f,screenHeight*5.0f,0.0f,-10.0f,10.0f);//glm::perspective(glm::radians(45.0f),(float)screenWidth/screenHeight,0.1f,100.0f);
    //backgroundProgram.setMatrix4fv("proj",glm::value_ptr(proj));

    //Planet::outlineProgram.init("../../resources/shaders/vertex/betterShader.h","../../resources/shaders/fragment/outlineShader.h",{4,1,1,1});
    SDL_ShowCursor(SDL_DISABLE);
    BasicRenderPipeline pipeline;
    pipeline.init("./shaders/gravityVertexShader.h","./shaders/gravityFragmentShader.h",{2,1,4,4});


    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            eventsEmpty = false;
            KeyManager::update(e);
            MouseManager::update(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        if (eventsEmpty)
        {
            KeyManager::update(e);
            MouseManager::update(e);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glDepthMask(false);
       //backgroundProgram.draw(GL_TRIANGLES,camera.getCenter());

        Game::update();

        //apple.update();
        //solar.update();

        //camera.recenter({player.rect.x + player.rect.z/2,player.rect.y + player.rect.a/2});
        camera.recenter(Game::getPlayer().getComponent<RectComponent>()->getCenter());
        //camera.recenter({screenWidth,screenHeight});
        //printRect(camera.getRect());
        //float fuelHeight = player.fuel/100.0*100;
        //PolyRender::requestRect(camera.toWorld({10,screenHeight - fuelHeight - 10,30,fuelHeight}),{1,0,0,1},true,0,1);
        //PolyRender::requestRect({0,0,screenWidth*5,screenHeight*5},{0,0,0,.2},true,0,0);

        ViewPort::update();
        SpriteManager::render();
        PolyRender::render();
        //Font::tnr.write(Font::wordProgram,convert(DeltaTime::deltaTime),0,320,0,1,{0,0,0});
        GLContext::update();
        eventsEmpty = true;
        DeltaTime::update();
        SDL_Delay(std::max(0,10 - DeltaTime::deltaTime));

    }
    return 0;
}
