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

    StarSystem solar;
    solar.init();
    //Planet earth = {{500,screenHeight},200};
    //earth.sprite = 0;//"./planets/grassPlanet.png";
    Player player;
    player.standingOn = solar.getPlanet(0);
    if (player.standingOn)
    player.rect = {player.standingOn->center.x,player.standingOn->center.y - player.standingOn->radius,20,20};
    Sprite background;
    background.init("sprites/blueplanet.png");
    RenderCamera camera;
    camera.init(screenWidth,screenHeight);
    //BackgroundProgram backgroundProgram;
    BasicRenderPipeline backgroundProgram;
    backgroundProgram.init("./shaders/backgroundShader.h","./shaders/starLightShader.h",{2});
    glUseProgram(backgroundProgram.program);
    glUniform1f(glGetUniformLocation(backgroundProgram.program,"radius"),320.0f);
    glUniform2f(glGetUniformLocation(backgroundProgram.program,"screenDimen"),screenWidth,screenHeight);
    glUniform2f(glGetUniformLocation(backgroundProgram.program,"origin"),320,320);
    glUniform4f(glGetUniformLocation(backgroundProgram.program,"starColor"),1,1,1,1);
    glUniform4f(glGetUniformLocation(backgroundProgram.program,"voidColor"),0,0,.5,1);



    //RenderProgram test()

    //glm::mat4 proj = glm::ortho(0.0f,screenWidth*5.0f,screenHeight*5.0f,0.0f,-10.0f,10.0f);//glm::perspective(glm::radians(45.0f),(float)screenWidth/screenHeight,0.1f,100.0f);
    //backgroundProgram.setMatrix4fv("proj",glm::value_ptr(proj));

    //Planet::outlineProgram.init("../../resources/shaders/vertex/betterShader.h","../../resources/shaders/fragment/outlineShader.h",{4,1,1,1});

    SDL_ShowCursor(SDL_DISABLE);
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
       backgroundProgram.draw(GL_TRIANGLES,camera.getCenter());
       //        glDepthMask(true);

        //SpriteManager::request(PlanetSpriteManager::PlanetSprites.getSprite(0),ViewPort::basicProgram,{glm::vec4(270,270,100,100),0});
        player.update(solar,camera);
        solar.update();

        camera.recenter({player.rect.x + player.rect.z/2,player.rect.y + player.rect.a/2});
        //camera.recenter({screenWidth,screenHeight});
        //printRect(camera.getRect());
        float fuelHeight = player.fuel/100.0*100;
        //PolyRender::requestRect(camera.toWorld({10,screenHeight - fuelHeight - 10,30,fuelHeight}),{1,0,0,1},true,0,1);
        //PolyRender::requestRect({0,0,screenWidth*5,screenHeight*5},{0,0,0,.2},true,0,0);

        ViewPort::update(&camera);
        SpriteManager::render();
        PolyRender::render();
        //Font::alef.write(Font::wordProgram,convert(DeltaTime::deltaTime),0,320,0,1,{0,0,0});
        GLContext::update();
        eventsEmpty = true;
        DeltaTime::update();
        SDL_Delay(std::max(0,10 - DeltaTime::deltaTime));

    }
    return 0;
}
