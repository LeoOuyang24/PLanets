#include "shaders.h"


 BackgroundProgram::BackgroundProgram() : RenderProgram("./shaders/backgroundShader.h","../../resources/shaders/fragment/blurShader.h")
{

    dataAmount = ViewPort::basicProgram->getRequestDataAmount(); //still need data to pass to the basic shader, although our own program doesn't need any additional data

    glGenFramebuffers(1,&fbo); //create frame buffer

    glBindFramebuffer(GL_FRAMEBUFFER,fbo);

    glGenTextures(1, &texture); //create the corresponding texture, which will store our colors
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ViewPort::getScreenDimen().x, ViewPort::getScreenDimen().y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //generate texture to match our screen dimensions

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //filtering, apparently necessary
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0); //attach texture to frame buffer

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "BackgroundProgram: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //initBuffers(); //load a different set of verticies to ensure our image is not rendered up side down

}

void BackgroundProgram::draw(Buffer sprite, void* data, int instances)
{


    glBindFramebuffer(GL_FRAMEBUFFER,fbo);

        RenderProgram::draw(sprite,0,1);
    for (int i = 0 ; i < 1; ++i)
    {
        //glClear(GL_COLOR_BUFFER_BIT);
        RenderProgram::draw(texture,0,1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);


    ViewPort::basicProgram->draw(texture,data,instances);
    //RenderProgram::draw(sprite,0,1);



    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
    //RenderProgram::draw(sprite,data,instances);

}
