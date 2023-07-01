#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED

#include "render.h"

class BackgroundProgram : public RenderProgram
{
    Buffer fbo;
    Buffer texture = 0;
    BasicRenderPipeline PostProcess; //used for post processing
public:
    BackgroundProgram();
    void draw(Buffer texture, void* data, int instances);
};

#endif // SHADERS_H_INCLUDED
