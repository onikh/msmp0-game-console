#include "Platforms.h"
#include "Player.h"
#include "../inc/ST7735.h"


const Platform_t Stage1[MAX_PLATFORMS] = {
    {0, 127, 160, 1, ST7735_GREEN},  // ground
    {60, 80, 40, 4, ST7735_YELLOW},
    {20, 100, 60, 4, ST7735_BLUE},    
    {90, 55, 50, 4, ST7735_RED}      
};


void Platform_Draw(void) {
    for(int i = 0; i < MAX_PLATFORMS; i++) {
        ST7735_FillRect(Stage1[i].x, Stage1[i].y, Stage1[i].w, Stage1[i].h, Stage1[i].color);
    }
}


void Platform_CheckCollision(Player_t *p) {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if ((p->xPos + 12 > Stage1[i].x) && (p->xPos + 4 < Stage1[i].x + Stage1[i].w)) {
            if (p->verticalVelocity <= 0) {
                if (p->prevY <= Stage1[i].y && p->yPos >= Stage1[i].y) {
                    p->yPos = Stage1[i].y;
                    p->verticalVelocity = 0;
                    p->animationState = 0; // grounded, clears jump state too
                    return;
                }
            }
        }
    }
}