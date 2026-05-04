#ifndef __TARGET_H__
#define __TARGET_H__


struct Target {
    int32_t xPos;
    int32_t yPos;
    int32_t prevX;
    int32_t prevY;
    uint8_t broken; //0 for unbroken, 1 for broken, direct index into array below
    int8_t xVelocity;
    int8_t yVelocity;
};
typedef struct Target Target_t;

void Target_Update(struct Target *t);
void RenderTarget(Target_t *t);
extern Target_t targets[];
extern const uint16_t TargetAnimation[2][256];;


#endif