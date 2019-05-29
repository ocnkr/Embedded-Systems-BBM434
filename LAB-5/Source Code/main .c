// main.c
// Runs on LM4F120 or TM4C123
// C2_Toggle_PF1, toggles PF1 (red LED) at 5 Hz
// Daniel Valvano, Jonathan Valvano, and Ramesh Yerraballi
// January 18, 2016

//*****************************************************************************
// define GPIO registers (PORTE)
// define SYSCTL_RCGC2_R
// define variables for SysTick and interrupt
//*****************************************************************************
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "Lab15_SpaceInvaders/Nokia5110.c"
#include <stdlib.h>


//Global variables
volatile unsigned int turn;
volatile unsigned int pressed;
volatile unsigned int timer;
volatile unsigned int Player1, Player2;
volatile unsigned int start_time;


//Function prototypes
void Switch_Init(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void SetWinner(void);					// Decides the winner.
void Delay(unsigned int deliminator);	//Delay function to wait a short time before the new turn.
void GPIOPortE_Handler(void);	// Switch Handler
void SysTick_Init(unsigned long period);	// SysTick initialization
void SysTick_Handler(void);		// SysTick Handler
void Switch_Init(void);		// Switch initialization

int main(void){ 	
	DisableInterrupts();
	Switch_Init();
	SysTick_Init(16000);	
	Nokia5110_Init();
	EnableInterrupts();
	while(1){
		Nokia5110_Clear();
		start_time = ((rand() % 10) + 1) * 1000; // random int between 0 and 9 plus 1
		turn = 0;
		timer = 0;
		Player1 = 0; 
		Player2 = 0;
		pressed = 0;
		
		Nokia5110_ClearBuffer();
		Nokia5110_SetCursor(0,0);	
		Nokia5110_DisplayBuffer();
		GPIO_PORTE_DATA_R = 0x00;
		Nokia5110_OutString("GET READY!");
		while(turn == 0){
			WaitForInterrupt();
		}
		Delay(50);
		EnableInterrupts();
		GPIO_PORTE_DATA_R = 0x00;		// turns off leds before the new turn
	}
}

/*
* Decides who is the winner.
* First, prints the values that shows users' press times.
* If it is lower than 100 ms, shows false start.
* If not, checks which of them is bigger.
* Bigger one will lose.
*/
void SetWinner(void){
	Nokia5110_Clear();
	Nokia5110_SetCursor(0,0);
	Nokia5110_DisplayBuffer();
	Nokia5110_OutString("RED:");
	Nokia5110_OutUDec(Player1);
	Nokia5110_OutString(" ms");
	Nokia5110_SetCursor(0,1);
	Nokia5110_OutString("GREEN:");
	Nokia5110_OutUDec(Player2);
	Nokia5110_OutString("ms");
	Nokia5110_SetCursor(0,2);
	if(Player1 <= 100){
			Nokia5110_Clear();
			Nokia5110_SetCursor(0,0);	
			Nokia5110_OutString("RED: false ");
			Nokia5110_SetCursor(0,1);
			Nokia5110_OutString("start");
			Nokia5110_SetCursor(0,2);
			Nokia5110_OutString("GREEN WINS!");
			GPIO_PORTE_DATA_R |= 0x08;
			GPIO_PORTE_DATA_R &= ~0x04;
	}
	else if(Player2 <= 100){
			Nokia5110_Clear();
			Nokia5110_SetCursor(0,0);
			Nokia5110_OutString("GREEN: false");
			Nokia5110_SetCursor(0,1);
			Nokia5110_OutString("start");
			Nokia5110_SetCursor(0,2);
			Nokia5110_OutString("RED WINS!");
			GPIO_PORTE_DATA_R |= 0x04;
			GPIO_PORTE_DATA_R &= ~0x08;
	}
	else {
		if(Player1 > Player2){
				Nokia5110_OutString("GREEN WINS");
				GPIO_PORTE_DATA_R |= 0x08;
				GPIO_PORTE_DATA_R &= ~0x04;
		}
		else{
			Nokia5110_OutString("RED WINS!");
			GPIO_PORTE_DATA_R |= 0x04;
			GPIO_PORTE_DATA_R &= ~0x08;
		}
	}
}

/*
* If player pressed after "Press" command, calculates the time when button is pressed.
* If times are bigger than 0, calls SetWinner() function.
* If player pressed before "Press" command, shows false start and the winner.
*/
void GPIOPortE_Handler(void){ 
	Nokia5110_Clear();
	Nokia5110_DisplayBuffer();
	// If pressed > 0, that means player pressed the button after "Press" command is displayed on the screen.
	if(pressed > 0){
		if(GPIO_PORTE_RIS_R & 0x01){
			GPIO_PORTE_ICR_R = 0x01;
			Player1 = timer - start_time;
		}
		if(GPIO_PORTE_RIS_R & 0x02){
			GPIO_PORTE_ICR_R = 0x02;
			Player2 = timer - start_time;
		}
		if(Player1 != 0 && Player2 != 0){
			SetWinner();
			turn++;
		}
	}
	// If pressed <= 0, that means player pressed the button before "Press" command is displayed on the screen.
	else{
		if(GPIO_PORTE_RIS_R & 0x01){
			GPIO_PORTE_ICR_R = 0x01;
			Nokia5110_Clear();
			Nokia5110_SetCursor(0,0);	
			Nokia5110_OutString("RED: false ");
			Nokia5110_SetCursor(0,1);
			Nokia5110_OutString("start");
			Nokia5110_SetCursor(0,2);
			Nokia5110_OutString("GREEN WINS!");
			GPIO_PORTE_DATA_R |= 0x08;
			GPIO_PORTE_DATA_R &= ~0x04;
		}
		if(GPIO_PORTE_RIS_R & 0x02){
			GPIO_PORTE_ICR_R = 0x02;
			Nokia5110_Clear();
			Nokia5110_SetCursor(0,0);
			Nokia5110_OutString("GREEN: false");
			Nokia5110_SetCursor(0,1);
			Nokia5110_OutString("start");
			Nokia5110_SetCursor(0,2);
			Nokia5110_OutString("RED WINS!");
			GPIO_PORTE_DATA_R |= 0x04;
			GPIO_PORTE_DATA_R &= ~0x08;
		}
		turn++;
	}
}

/* 
* SysTick handler will be operated after every 1 ms which we set when initialization of SysTick.
*/
void SysTick_Handler(void){
	timer++;															
	if(timer == start_time){
		Nokia5110_Clear();
		Nokia5110_SetCursor(0,0);	
		Nokia5110_DisplayBuffer();
		Nokia5110_OutString("PRESS");
		pressed++;
	}
}

void Delay(unsigned int deliminator){
	unsigned long volatile time;
	time = deliminator*160000;
	DisableInterrupts();
	while(time){
		time--;
	}
}

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
	NVIC_ST_RELOAD_R = period-1;// reload value
	NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2          
	NVIC_ST_CTRL_R = 0x07; 			// enable SysTick with core clock and interrupts
}

void Switch_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     		// activate clock for Port E
	delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   		// unlock GPIO Port E
	GPIO_PORTE_CR_R = 0x0F;           		// allow changes to PE0-3
	GPIO_PORTE_AMSEL_R = 0x00;        		// disable analog on PE
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF; 		// configure PE3-0 as GPIO
	GPIO_PORTE_DIR_R = 0x0C;          		// PE0-1 in, PE2-3 out
	GPIO_PORTE_AFSEL_R = 0x00;        		// disable alt funct on PE7-0
	GPIO_PORTE_DEN_R = 0x0F;          		// enable digital I/O on PE3-0
	GPIO_PORTE_IS_R &= ~0x03;     				// PE0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x03;    				// PE0 is not both edges
  GPIO_PORTE_IEV_R &= ~0x03;    				// PE0 falling edge event
  GPIO_PORTE_ICR_R = 0x03;      				// clear flag0
  GPIO_PORTE_IM_R |= 0x03;      				// arm interrupt on PE0
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000020; // priority 1
  NVIC_EN0_R = 0x00000010;      				// enable interrupt 4 in NVIC
}
