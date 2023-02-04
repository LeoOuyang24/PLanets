#include <iostream>
#include <time.h>
#include <SDL.h>

#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"

#include "planet.h"
#include "player.h"
#include "starSystem.h"

int main(int args, char* argsc[])
{
    //delete ptr;
    const int screenWidth = 640;
    const int screenHeight = 640;
    srand(time(NULL));
    GLContext::init(screenWidth,screenHeight);
    SDL_StopTextInput();
    ViewPort::init(screenWidth,screenHeight);
    Font::init(screenWidth, screenHeight);
    PolyRender::init(screenWidth,screenHeight);
    SDL_Event e;
    bool quit = false;
    glClearColor(0,0,0,0);
    bool eventsEmpty = true;
        //std::cout << tree.count() << std::endl;
    PlanetSprites.init();

    StarSystem solar;
    solar.init();
    //Planet earth = {{500,screenHeight},200};
    //earth.sprite = 0;//"./planets/grassPlanet.png";
    Player player;
    player.standingOn = solar.getPlanet(0);
    player.rect = {player.standingOn->center.x,player.standingOn->center.y - player.standingOn->radius,20,20};

    Sprite background;
    background.init("sprites/starless_background.png");
    RenderCamera camera;
    camera.init(screenWidth,screenHeight);

    RenderProgram blurProgram;
    blurProgram.init("../../resources/shaders/vertex/betterShader.h","../../resources/shaders/fragment/blurShader.h",7,{4,1,1,1});

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

       // earth.render();
        player.update(solar);
        solar.update();

        camera.recenter({player.rect.x + player.rect.z/2,player.rect.y + player.rect.a/2});
        //camera.recenter({screenWidth,screenHeight});
        //printRect(camera.getRect());
        SpriteManager::request(background,blurProgram,{{0,0,screenWidth*5,screenHeight*5},0},0);
        float fuelHeight = player.fuel/100.0*100;
        PolyRender::requestRect(camera.toWorld({10,screenHeight - fuelHeight - 10,30,fuelHeight}),{1,0,0,1},true,0,1);

        ViewPort::update(&camera);
        SpriteManager::render();
        PolyRender::render();
       // Font::alef.write(Font::wordProgram,"asdf",320,320,0,1,{0,0,0});
        GLContext::update();
        eventsEmpty = true;
        DeltaTime::update();
        SDL_Delay(std::max(0,10 - DeltaTime::deltaTime));

        //std::cout << DeltaTime::deltaTime << std::endl;
    }
    return 0;
}
