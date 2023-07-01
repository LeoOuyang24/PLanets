#ifndef FOLLOWER_H_INCLUDED
#define FOLLOWER_H_INCLUDED

#include "player.h"

struct Follower
{
    Player* player = 0;
    Sprite* sprite = 0;
    glm::vec4 rect;
    Follower(const glm::vec4& rect, Player* player_, Sprite* sprite_);
    void update();
};

#endif // FOLLOWER_H_INCLUDED
