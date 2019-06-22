#ifndef MAIN_H
#define MAIN_H

#include "tm4c123gh6pm.h"     // tm4c123gh6pm register definitions
#include "PLL.h"              // configuration for 80 MHz clock
#include "SysTick.h"          // Systick periodic interrupts and delay functions
#include "Timers.h"           // timer functions and handlers
#include "EdgeInterrupts.h"   // edge interrupt functions and handlers
#include "OmniStructure.h"    // omni data structures 
#include "OmniControl.h"      // omnidirectional robot control function
#include "Ultrasonic.h"       // ultrasonic sensor functions

#include <stdbool.h>          // included to use boolean data type
#include <math.h>             // standard C math library
#include <stdint.h>           // C99 variable types
#include <stdio.h>            // standard C input output library
#endif /* MAIN_H */
