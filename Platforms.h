#ifndef __PLATFORMS_H__
#define __PLATFORMS_H__

#include <stdint.h>
#include <Player.h>

typedef struct {
    int16_t x;      
    int16_t y;      
    int16_t w;      
    int16_t h;      
    uint16_t color; 
} Platform_t;


#define MAX_PLATFORMS 4


extern const Platform_t Stage1[MAX_PLATFORMS];

// Function prototypes
void Platform_Init(void);
void Platform_Draw(void);
void Platform_CheckCollision(Player_t *p);

#endif