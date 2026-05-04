#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"





void ButtonInt_Init(void) {

    IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PB16INDEX] = 0x00040081;

    GPIOB->POLARITY31_16 = 0x00000005;

    GPIOB->CPU_INT.ICLR = (1<<17) | (1<<16); // clearing pedning interrupts
    GPIOB->CPU_INT.IMASK = (1<<17) | (1<<16); // enabling interrupts on both pins


    NVIC->IP[0] |= (2<<14);
    NVIC->ISER[0] |= (1<<1);
}



