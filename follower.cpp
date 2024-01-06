#include "follower.h"

Follower::Follower(const glm::vec4& rect_, Player* player_, Sprite* sprite_) : rect(rect_),player(player_),sprite(sprite_)
{

}

void Follower::update()
{
    if (player && sprite)
    {
        SpriteManager::request(*sprite,*ViewPort::basicProgram,{rect,1});
        glm::vec2 norm = betterNormalize(glm::vec2(rect.x,rect.y)-glm::vec2(player->rect.x,player->rect.y));
        rect.x = player->rect.x + player->rect.z/2 + norm.x*10;
        rect.y = player->rect.y + player->rect.a/2 + norm.y*10;
    }
}
