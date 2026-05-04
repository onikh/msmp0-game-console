#include "./Player.h"
#include "stdint.h"

void Player_Init(Player_t *p) {
    p->xPos = 80 - 8;
    p->yPos = 126;
    p->animationState = 0;
    p->verticalVelocity = 0;
    p->prevX = 40;
    p->prevY = 40;
    p->score = 0;
    p->direction = 0;
   // p->isGrounded = 0;
}

void Player_Update(Player_t *p, uint32_t xIn, uint32_t yIn, uint32_t buttonIn) {
    p->prevX = p->xPos;
    p->prevY = p->yPos;

    // --- Horizontal movement ---
    if ((xIn > 2500) && (p->xPos < 160 - 16)) {
        p->xPos += (((xIn - 2500) / 300) + 1);
        if (p->animationState != 2) p->animationState = 1;
        p->direction = 0;
    } else if ((xIn < 1400) && (p->xPos > 0)) {
        p->xPos -= (((1400 - (int32_t)xIn) / 260) + 1);
        if (p->animationState != 2) p->animationState = 1;
        p->direction = 1;
    } else if (p->animationState != 2) {
        p->animationState = 0;
    }

    
    p->yPos -= p->verticalVelocity;
    p->verticalVelocity -= 1;


    if (p->yPos >= 125) {
        p->verticalVelocity = 0;
        p->yPos = 125;
        if (p->animationState == 2) p->animationState = 0; 
    }

    if (p->xPos < 0) p->xPos = 0;
    if (p->xPos > 159) p->xPos = 159;
}

