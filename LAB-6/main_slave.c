// main.c
// Runs on LM4F120 or TM4C123
// Onur Cankur, Merve Müge Deliktas
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
void Switch_LED_Init(void);		// Switch initialization

void PLL_Init(void){
  // 0) Use RCC2
  SYSCTL_RCC2_R |=  0x80000000;  // USERCC2
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |=  0x00000800;  // BYPASS2, PLL bypass
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0)   // clear XTAL field, bits 10-6
                 + 0x00000540;   // 10101, configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~0x00000070;  // configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~0x00002000;
  // 4) set the desired system divider
  SYSCTL_RCC2_R |= 0x40000000;   // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000)  // clear system clock divider
                  + (4<<22);      // configure for 80 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&0x00000040)==0){};  // wait for PLLRIS bit
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~0x00000800;
}

// Assumes a 80 MHz bus clock, creates 115200 baud rate
void UART_Init(void){            // should be called only once
  SYSCTL_RCGC1_R |= 0x00000002;  // activate UART1
  SYSCTL_RCGC2_R |= 0x00000004;  // activate port C
  UART1_CTL_R &= ~0x00000001;    // disable UART
  UART1_IBRD_R = 6;     // IBRD = int(80,000,000/(16*115,200)) = int(43.40278)
  UART1_FBRD_R = 5;     // FBRD = round(0.40278 * 64) = 26
  UART1_LCRH_R = 0x00000070;  // 8 bit, no parity bits, one stop, FIFOs
  UART1_CTL_R |= 0x00000001;     // enable UART
  GPIO_PORTC_AFSEL_R |= 0x30;    // enable alt funct on PC5-4
  GPIO_PORTC_DEN_R |= 0x30;      // configure PC5-4 as UART1
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;   // disable analog on PC5-4
}

// Wait for new input, then return ASCII code
unsigned char UART_InChar(void){
  while((UART1_FR_R&0x0010) != 0);      // wait until RXFE is 0
  return((unsigned char)(UART1_DR_R&0xFF));
}

// Wait for buffer to be not full, then output
void UART_OutChar(unsigned char data){
  while((UART1_FR_R&0x0020) != 0);      // wait until TXFF is 0
  UART1_DR_R = data;
}

// Immediately return input or 0 if no input
unsigned char UART_InCharNonBlocking(void){
  if((UART1_FR_R&UART_FR_RXFE) == 0){
    return((unsigned char)(UART1_DR_R&0xFF));
  } else{
    return 0;
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
int i = 0;
unsigned char a;
int main(void){ 	
	DisableInterrupts();
	PLL_Init();
	Switch_LED_Init();
	UART_Init();
	Nokia5110_Init();
	Nokia5110_Clear();
	EnableInterrupts();
	while(1){
		WaitForInterrupt();
	}
}

void GPIOPortE_Handler(void){ 
	a = UART_InCharNonBlocking();
	if(a != 0){
		Nokia5110_SetCursor(i,0);
		i++;
		Nokia5110_OutChar(a);
	}
}

void Switch_LED_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     		// activate clock for Port E
	delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   		// unlock GPIO Port E
	GPIO_PORTE_CR_R = 0x05;           		// allow changes to PE0-3
	GPIO_PORTE_AMSEL_R = 0x00;        		// disable analog on PE
	//GPIO_PORTF_PDR_R = 0x01;
	GPIO_PORTE_PCTL_R &= ~0x0000000F; 		// configure PE3-0 as GPIO
	GPIO_PORTE_DIR_R = 0x04;          		// PE0-1 in, PE2-3 out
	GPIO_PORTE_AFSEL_R = 0x00;        		// disable alt funct on PE7-0
	GPIO_PORTE_DEN_R = 0x05;          		// enable digital I/O on PE3-0
	GPIO_PORTE_IS_R &= ~0x01;     				// PE0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x01;    				// PE0 is not both edges
  GPIO_PORTE_IEV_R &= ~0x01;    				// PE0 falling edge event
  GPIO_PORTE_ICR_R = 0x01;      				// clear flag0
  GPIO_PORTE_IM_R |= 0x01;      				// arm interrupt on PE0
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000020; // priority 1
  NVIC_EN0_R = 0x00000010;      				// enable interrupt 4 in NVIC
}
