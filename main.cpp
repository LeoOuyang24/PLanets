#include <iostream>
#include <time.h>
#include <SDL.h>

#include "render.h"
#include "SDLHelper.h"
#include "FreeTypeHelper.h"

#include "planet.h"
#include "player.h"

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


    Planet earth = {{500,screenHeight},200};
    earth.sprite.init("sprites/grassPlanet.png");
    Player player;
    player.rect = {earth.center.x,earth.center.y - earth.radius,20,20};
    player.standingOn = &earth;
    Sprite background;
    background.init("sprites/starless_background.png");
    RenderCamera camera;
    camera.init(screenWidth,screenHeight);
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

        earth.render();
        player.update();

        camera.recenter({player.rect.x + player.rect.z/2,player.rect.y + player.rect.a/2});
        //camera.recenter({screenWidth,screenHeight});
        //printRect(camera.getRect());
        SpriteManager::request(background,ViewPort::basicProgram,{{0,0,screenWidth*5,screenHeight*5},0},0);

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
