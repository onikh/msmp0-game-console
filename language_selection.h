#ifndef __LANGUAGE_SELECTION_H__
#define __LANGUAGE_SELECTION_H__

#include <stdint.h>

extern uint32_t language_flag;  // 0 = English, 1 = Tagalog

void Buttons_Init(void);
void WaitForButtonPress(void);

extern const uint16_t TitleScreen[];

#endif