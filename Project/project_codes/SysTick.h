#ifndef SYSTICK_H
#define SYSTICK_H

#include "MacroDefinitions.h"

/** @brief  Initialize Systick periodic interrupts
  * @input  Interrupt period
  * @output None
  */
void SysTick_Init(unsigned long period);

/** @brief  Executed every (Period / CLOCK_FREQ) sec
  * @input  None
  * @output None
  */
void SysTick_Handler(void);

/** @brief  Delay function in seconds
  * @input  Seconds
  * @output None
  */
void delay_sec(unsigned long time);

/** @brief  Delay function in miliseconds
  * @input  Miliseconds
  * @output None
  */
void delay_ms(unsigned long time);

/** @brief  Delay function in microseconds
  * @input  Microseconds
  * @output None
  */
void delay_us(unsigned long time);

/* Functions that are defined in startup.s file */
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern unsigned char SineWave2_High[32];
extern unsigned char SineWave2_Low[32];
extern unsigned char Index2; // Index varies from 0 to 32
extern unsigned int DO2;	
extern unsigned int RE2;
extern unsigned int MI2;
extern unsigned int FA2;
extern unsigned int SOL2;
extern unsigned int LA2;
extern unsigned int SI2;
extern unsigned int DO_2;
#endif /* SYSTICK_H */
