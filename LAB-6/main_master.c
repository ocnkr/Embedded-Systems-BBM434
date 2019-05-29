// main.c
// Runs on LM4F120 or TM4C123
// C2_Toggle_PF1
// Kutay Barcin, Defne Tuncer
// April 11, 2019

#include "tm4c123gh6pm.h"

void EnableInterrupts();
void DisableInterrupts();
void WaitForInterrupt();

#define PE4 (*((volatile unsigned long *)0x40024040))
#define PE5 (*((volatile unsigned long *)0x40024080))

void PortE_Init_2345(void){volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x00000010;			// activate clock for Port E
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_AMSEL_R &= ~0x3C;  				// disable analog on PE2-5
	GPIO_PORTE_PCTL_R &= ~0x00FFFF00; 		// PCTL GPIO on PE2-5
	GPIO_PORTE_DIR_R = 0x30;  						// PE2 PE3 input PE4 PE5 output
	GPIO_PORTE_AFSEL_R &= ~0x3C; 					// PE2-5 regular port function
	GPIO_PORTE_DEN_R |= 0x3C; 						// enable PE2-5 digital port
}

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

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;          // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1; // reload value
  NVIC_ST_CURRENT_R = 0;       // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0xA0000000; // priority 5
  NVIC_ST_CTRL_R = 0x07;       // enable SysTick with core clock and interrupts
}

int pos = 0;
int binary[8] = {0,0,0,0,0,0,0,0};
int stop = 1;

void charToBin(char c){
		if(c=='\0') {
			stop=0;
			PE4=0x00;
		}
    for(pos = 0; pos < 8; pos++){
        binary[pos] = (c >> pos) & 1;
    }}

//char message[] = "SELMA  HOCA ROCKS";
int d=0;
char message[] = "ABCDEFGHIJKLMNOPRSTUVYZ1234567890";
int i = 0;
		
void SysTick_Handler(void){
	if(stop){
		PE4^=0x10;
		if(PE4&0x10) {
			if(binary[i]) PE5=0x20;
			else PE5=0x00;
			i++;
			if(i==8){
				charToBin(message[d]);
				d++;
				i=0;
			}
		}
	}
}

int main(void){
	charToBin(message[d]);
	d++;
	PLL_Init();
	PortE_Init_2345();
	SysTick_Init(10100);
	EnableInterrupts();
	while(1){}
}
