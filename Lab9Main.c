// Lab9Main.c
// Runs on MSPM0G3507
// Lab 9 ECE319K
// Your name
// Last Modified: January 12, 2026

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "../inc/Arabic.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"

#include "./language_selection.h"
#include "./Player.h"
#include "./ButtonInt.h"
#include "./Target.h"
#include "./Platforms.h"


// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz


const char dialogue[2][3][10] = {
  {"SCORE", "TIME", "You win!"},
  {"PUNTOS", "ORASAN", "Panalo Ka!"},
  };
uint8_t languageIndex;
  

Player_t player;

//Animation handling
volatile uint8_t animationIndex = 0;
const struct animation* currentAnimation = animationTable[RUN];



//Scorekeeping
uint8_t score = 0;
uint8_t renderScore = 0; //make 1 when score needs to be updated

// globals for pausing
volatile uint8_t paused = 0;
volatile uint32_t timerTicks = 0;
volatile uint32_t elapsedSeconds = 0;
uint32_t lastDisplayedSeconds = 0xFFFFFFFF;


static uint8_t PlayerHitsTarget(Player_t *p, Target_t *t){
  int32_t pLeft   = p->xPos;
  int32_t pRight  = p->xPos + 15;
  int32_t pTop    = p->yPos;
  int32_t pBottom = p->yPos - 15;

  int32_t tLeft   = t->xPos;
  int32_t tRight  = t->xPos + 15;
  int32_t tTop    = t->yPos;
  int32_t tBottom = t->yPos - 15;

  // no overlap cases
  if(pRight < tLeft)   return 0;
  if(pLeft > tRight)   return 0;
  if(pTop < tBottom)   return 0;
  if(pBottom > tTop)   return 0;

  return 1;
}


void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

Arabic_t ArabicAlphabet[]={
alif,ayh,baa,daad,daal,dhaa,dhaal,faa,ghayh,haa,ha,jeem,kaaf,khaa,laam,meem,noon,qaaf,raa,saad,seen,sheen,ta,thaa,twe,waaw,yaa,zaa,space,dot,null
};
Arabic_t Hello[]={alif,baa,ha,raa,meem,null}; // hello
Arabic_t WeAreHonoredByYourPresence[]={alif,noon,waaw,ta,faa,raa,sheen,null}; // we are honored by your presence
int main0(void){ // main 0, demonstrate Arabic output
  Clock_Init80MHz(0);
  LaunchPad_Init();
  ST7735_InitR(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(ST7735_WHITE);
  Arabic_SetCursor(0,15);
  Arabic_OutString(Hello);
  Arabic_SetCursor(0,31);
  Arabic_OutString(WeAreHonoredByYourPresence);
  Arabic_SetCursor(0,63);
  Arabic_OutString(ArabicAlphabet);
  while(1){
  }
}
uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}


// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites

    // 4) start sounds
    // 5) set semaphore
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    //animationIndex++;

        if(!paused){
      timerTicks++;
      if (timerTicks >= 30) {
        timerTicks = 0;
        elapsedSeconds++;
      }
    }


    if (!paused) {
    Player_Update(&player, ADCin(), 0, 0);
    
    Platform_CheckCollision(&player);
    currentAnimation = animationTable[player.animationState];

    for(int i = 0; i < 8; i++) {
    //if (targets[i].xPos != targets[i].prevX || targets[i].yPos != targets[i].prevY) {
        
        if (!targets[i].broken) { //if target isn't broken
        if (PlayerHitsTarget(&player, &targets[i]))  {
          targets[i].broken = 1;
          targets[i].yVelocity = 0;
          targets[i].xVelocity = 0;
          score += 1;
          renderScore = 1;
          Sound_TargetBroken();
        }
        Target_Update(&targets[i]);
        }
    }
    } //if for pause condition



    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}


void TIMG0_IRQHandler(void) {
  animationIndex++;
}

void GROUP1_IRQHandler(void) {
  uint32_t mis = GPIOB->CPU_INT.MIS;

  if (mis & (1<<17) && !paused) {
    GPIOB->CPU_INT.ICLR = (1<<17);
    if (player.animationState != JUMP) {
      player.verticalVelocity = 7;
      player.animationState = JUMP;
      Sound_Jump();
    }
  }

  if (mis & (1<<16)) {
    GPIOB->CPU_INT.ICLR = (1<<16);
    paused = !paused;
  }
}

uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(0x0000);            // set screen to black
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
 /* ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10); */

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs
    
  }
}


// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}



// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main5(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(ST7735_BLACK);
  ADCinit();     //PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(80000000/30,2);
  // initialize all data structures
  __enable_irq();

  while(1){
    // wait for semaphore
       // clear semaphore
       // update ST7735R
    // check for end game or level switch
  }
}




int main(void) {
   __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_BLACKTAB);
  ST7735_SetRotation(1);


  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(40, 45, TitleScreen, 80, 30);
  ST7735_SetCursor(2, 7);
  ST7735_OutString("Top button");
  ST7735_SetCursor(2, 8);
  ST7735_OutString("->English");
  ST7735_SetCursor(2, 10);
  ST7735_OutString("Ibabang pindutan");
  ST7735_SetCursor(2, 11);
  ST7735_OutString("-> Tagalog");


  Buttons_Init();
  WaitForButtonPress();

  ST7735_FillScreen(ST7735_BLACK);
  if (language_flag == 0) {
      ST7735_SetCursor(3, 4);
      ST7735_OutString("English");
      ST7735_SetCursor(2, 6);
      ST7735_OutString("Get Ready!");
  } else {
      ST7735_SetCursor(3, 4);
      ST7735_OutString("Tagalog");
      ST7735_SetCursor(2, 6);
      ST7735_OutString("Handa Ka Na!");
  }
  Clock_Delay1ms(1500);



  ADCinit();

  TimerG12_IntArm(80000000/30,2);
  TimerG0_IntArm(38527, 500, 1);
  ButtonInt_Init();

  Player_Init(&player);
  Sound_Init();   
  __enable_irq();

  ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0, 0);
  if (language_flag == 0) {
    ST7735_OutString("SCORE: ");
  } else {
    ST7735_OutString("PUNTOS:");
  }
  ST7735_OutUDec(score);
  ST7735_OutString("/5");

  ST7735_SetCursor(0, 1);
  if (language_flag == 0) {
    ST7735_OutString("TIME: 0:00");
  } else {
    ST7735_OutString("ORAS: 0:00");
  }
  ST7735_SetCursor(0, 5);

  //ST7735_FillScreen(ST7735_BLACK);
   // Draw the level once
  
  // ... rest of your UI strings ...
  
  
  while (1) {

    if ( score >= 8) {
      break;
    }

    if (!paused) {
  
        // timer display
      if (elapsedSeconds != lastDisplayedSeconds) {
        lastDisplayedSeconds = elapsedSeconds;
        uint32_t minutes = elapsedSeconds / 60;
        uint32_t seconds = elapsedSeconds % 60;
        ST7735_SetCursor(6, 1);       // after "TIME: "
        ST7735_OutUDec(minutes);
        ST7735_OutChar(':');
        if (seconds < 10) ST7735_OutChar('0');
        ST7735_OutUDec(seconds);
      }

      //render platforms here
      Platform_Draw();

      for(int i = 0; i < 8; i++) {
      //if (targets[i].xPos != targets[i].prevX || targets[i].yPos != targets[i].prevY) {
        
          RenderTarget(&targets[i]);
      //}
      }

      if (player.xPos > player.prevX) {
        ST7735_FillRect(player.prevX, player.prevY - 15, player.xPos - player.prevX, 16, ST7735_BLACK);
      } else if (player.xPos < player.prevX) {
        ST7735_FillRect(player.xPos + 16, player.prevY - 15, player.prevX - player.xPos, 16, ST7735_BLACK);
      }

      if (renderScore) {
        ST7735_SetCursor(7, 0);
        ST7735_OutUDec(score);
        renderScore = 0;
      }

      // Y delta clipping
      if (player.yPos > player.prevY) {

      ST7735_FillRect(player.xPos, player.prevY - 15, 16, player.yPos - player.prevY, ST7735_BLACK);
      } else if (player.yPos < player.prevY) {

      ST7735_FillRect(player.xPos, player.yPos + 1, 16, player.prevY - player.yPos, ST7735_BLACK);
      }

      if (player.direction == 1) {
        ST7735_DrawBitmapReversed(player.xPos, player.yPos, currentAnimation->frames[animationIndex%currentAnimation->numberFrames], 16,16); // x,y,name,width,height  x,y is bottom left corner of image
      } else {
        ST7735_DrawBitmap(player.xPos, player.yPos, currentAnimation->frames[animationIndex%currentAnimation->numberFrames], 16,16);
      }
      //ST7735_DrawBitmap(64, 16, Idle.frames[animationIndex%4], 16,16); 
    } //else {
    //  ST7735_OutString("PAUSED");
    //}
  }

   ST7735_SetCursor(6, 3);
   ST7735_OutString("You win!");

  ST7735_SetCursor(6, 3);
   if (language_flag == 1) ST7735_OutString("Panalo ka!");
}