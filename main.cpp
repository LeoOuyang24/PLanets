#include <iostream>
#include <time.h>
#include <SDL.h>

#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"
#include "resourcesMaster.h"

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

    ResourcesConfig::loadConfig();

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

    Sprite bunny("sprites/bunny.png");

    PlanetSprites.init();
    Game::init();

//REFACTOR: Make it so functions that need solar system just get it from Game rather than getting it passed in.



    /*Sprite chomper ("sprites/chomperSheet.png");
    Entity* rabbit = new Entity();
    rabbit->addComponent(*(new MoveOnPlanetComponent(*Game::getSolar().getPlanet(0),{10,10},{},*rabbit)));
    //rabbit.addComponent(*(new RabbitAI(rabbit)));
    rabbit->addComponent(*(new EntityAnimationComponent(*rabbit,chomper,GAME_Z)));
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
    spitter->addComponent(*(new MoveOnPlanetComponent(*Game::getSolar().getPlanet(0),{30,30},{},*spitter)));
    //rabbit.addComponent(*(new RabbitAI(rabbit)));
    spitter->addComponent(*(new EntityAnimationComponent(*spitter,spitter_sprite,GAME_Z,{BaseAnimation::normalizePixels({0,0,1,1},spitter_sprite),4,2,8})));
    //rabbit.addComponent(*(new RectRenderComponent(rabbit,{1,1,1,1})));

    /*Entity* solarian = new Entity();
    solarian->addComponent(*(new BasicMoveComponent(glm::vec4(100,100 ,30,30),*solarian)));
    solarian->addComponent(*(new AIComponent(*solarian,AIStateBase::IDLE,*(new IdleState(1000,AIStateBase::PATROL)),*(new AerialWanderFunc()))));
    solarian->addComponent(*(new SolarianRenderComponent(*solarian)));*/
    //BaseAnimationComponent comp(rabbit,anime);

    Sprite rocket("sprites/teleporter.png");
    std::shared_ptr<Entity> launchpad = std::shared_ptr<Entity>((new Entity()));
    launchpad->addComponent(*(new MoveOnPlanetComponent(*Game::getSolar().getPlanet(0),{50,30},{},*launchpad)));
    launchpad->addComponent(*(new EntityAnimationComponent(*launchpad,rocket)));
    Planet* lastPlanet = Game::getSolar().getPlanet(Game::getSolar().size() - 1);
    launchpad->addComponent(*(new LaunchComponent(3,0,*launchpad)));


    //Game::getManager().addEntity(*rabbit);
    //Game::getManager().addEntity(*solarian);
    //Game::getManager().addEntity(*spitter);
    Game::getManager().addMasterListEntry(launchpad);

    Game::getManager().addEntitiesFromLayer(0);

    Sprite background;
    background.init("sprites/blueplanet.png");
    RenderCamera camera;
    ViewPort::setZRange((CAMERA_Z - GAME_Z)/2,CAMERA_Z);
    camera.init({0,0,CAMERA_Z});


    ViewPort::currentCamera = &camera;
    ViewPort::flipProj();
    //BackgroundProgram backgroundProgram;


    //Follower apple({30,30,30,30},&player,&appleSprite);


    //RenderProgram test()

    //glm::mat4 proj = glm::ortho(0.0f,screenWidth*5.0f,screenHeight*5.0f,0.0f,-10.0f,10.0f);//glm::perspective(glm::radians(45.0f),(float)screenWidth/screenHeight,0.1f,100.0f);
    //backgroundProgram.setMatrix4fv("proj",glm::value_ptr(proj));

    //Planet::outlineProgram.init("../../resources/shaders/vertex/betterShader.h","../../resources/shaders/fragment/outlineShader.h",{4,1,1,1});
    SDL_ShowCursor(SDL_DISABLE);
    BasicRenderPipeline starLight("./shaders/gravityVertexShader.h","./shaders/starLightShader.h");

    BasicRenderPipeline stars({LoadShaderInfo{"./shaders/gravityVertexShader.h",GL_VERTEX_SHADER,true},{"./shaders/starShader.h",GL_FRAGMENT_SHADER,true}});
    //BasicRenderPipeline stars("./shaders/gravityVertexShader.h","./shaders/starShader.h");
    //stars.init("./shaders/gravityVertexShader.h","./shaders/starShader.h");


    //stars.init();

    //RenderProgram balls("./shaders/distanceVertexShader.h","./shaders/distanceFragmentShader.h");

    //glDisable(GL_DEPTH_TEST);


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
        MoveOnPlanetComponent* moveOnPlanet = Game::getPlayer().getComponent<MoveOnPlanetComponent>();
        if (moveOnPlanet)
        {
            glm::vec3 targetPoint = glm::vec3(moveOnPlanet->getCenter(),CAMERA_Z - GAME_Z + StarSystem::getPlanetZGivenLayer(moveOnPlanet->getLayer()));
            camera.addVector(std::min(glm::length(targetPoint - camera.getPos()),0.005f*DeltaTime::deltaTime)*(targetPoint - camera.getPos()));
            //camera.setPos(targetPoint);
        }
        SpriteManager::request({stars},camera.getPos().z + FAR_Z - CAMERA_Z +1,true,glm::vec3(500,500,camera.getPos().z + FAR_Z - CAMERA_Z +1),500.0f,glm::vec4(1,1,1,1),glm::vec4(0,1,1,0));
        SpriteManager::request({starLight},FAR_Z,true,glm::vec3(500,500,FAR_Z),1000.0f,glm::vec4(1,1,1,1),glm::vec4(0,1,1,0));

        Game::update();

        ViewPort::update();
        SequenceManager::run();
        PolyRender::render();

        SpriteManager::render();
        //Font::tnr.write(Font::wordProgram,convert(DeltaTime::deltaTime),0,320,0,1,{0,0,0});
        GLContext::update();
        eventsEmpty = true;
        DeltaTime::update();
        //std::cout << DeltaTime::deltaTime << "\n";
        SDL_Delay(std::max(0,10 - DeltaTime::deltaTime));

    }
    return 0;
}
