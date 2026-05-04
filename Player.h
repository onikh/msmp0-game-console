

#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <stdint.h>



struct Player {
    uint8_t animationState; 
    int8_t verticalVelocity;
    int32_t xPos, yPos;
    uint8_t prevX;
    uint8_t prevY;
    uint8_t score;
    uint8_t direction;
 //   uint8_t isGrounded;
};
typedef struct Player Player_t;


void Player_Init(Player_t* p);
void Player_Update(Player_t* p ,uint32_t xIn, uint32_t yIn, uint32_t buttonIn);




#endif