#ifndef OMNICONTROL_H
#define OMNICONTROL_H

#include <stdbool.h> 
#include "OmniStructure.h" 
#include "MacroDefinitions.h"

extern volatile unsigned long long Counts;  // defined in SysTick.c (increments every 1 us)

/** @brief  Timer tasks to be called in timer handlers
  * @input  timerNum: Determines which timer function to be selected
  * @output None
  */
void Timer_Task(unsigned long timerNum);

/* Timer tasks to be called in timer handlers */
void Timer0_Task(void);
void Timer1_Task(void);
void Timer2_Task(void);
void Timer3_Task(void);	
void Timer4_Task(void);

/** @brief  Timer initializations for PID loops
  * @input  None
  * @output None
  */
void OmniControl_Init(void);
void DAC_Out(unsigned long data);

#endif /* OMNICONTROL_H */
