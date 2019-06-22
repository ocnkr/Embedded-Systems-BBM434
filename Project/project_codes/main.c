#include "main.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

void EnableInterrupts(void);
void WaitForInterrupt(void);
void Sound_Init(void);
void SysTick_Init(unsigned long count);
void SysTick_Handler(void);
void DAC_Init(void);
void DAC_Out(unsigned long data);
void DAC_Init2(void);
unsigned char SineWave2_High[32] = {8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};
unsigned char SineWave2_Low[32] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
unsigned char Index2; // Index varies from 0 to 31
unsigned int DO2 = 9542;		
unsigned int RE2 = 8503;
unsigned int MI2 = 7575;
unsigned int FA2 = 7163;
unsigned int SOL2 = 6377;
unsigned int LA2 = 5681;
unsigned int SI2 = 5060;
unsigned int DO_2 = 4780;
																		
void Delay(int delay){
	while(delay > 0){
		delay--;
	}
}

void Buttons_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R|=0x00000010;      // activate clock for Port F
	delay=SYSCTL_RCGC2_R;            // allow time for clock to start
	GPIO_PORTE_CR_R|=0x3E;           // allow changes to PF4-0
	GPIO_PORTE_AMSEL_R|=0x00;         // disable analog
	GPIO_PORTE_PCTL_R|=0;             // used when there is an alternative function
	GPIO_PORTE_DIR_R|=0x00;           // PF4-0 INPUT PF1 2 3 OUTPUT
	GPIO_PORTE_AFSEL_R|=0x00;         // disable alternative functions
	GPIO_PORTE_DEN_R|=0x3E;           // enable digital I/O on Port F
	
	SYSCTL_RCGC2_R|=0x00000001;      // activate clock for Port F
	delay=SYSCTL_RCGC2_R;            // allow time for clock to start
	GPIO_PORTB_CR_R|=0xC8;           // allow changes to PF4-0
	GPIO_PORTB_AMSEL_R|=0x00;         // disable analog
	GPIO_PORTB_PCTL_R|=0;             // used when there is an alternative function
	GPIO_PORTB_DIR_R|=0x00;           // PF4-0 INPUT PF1 2 3 OUTPUT
	GPIO_PORTB_AFSEL_R|=0x00;         // disable alternative functions
	GPIO_PORTB_DEN_R|=0xC8;           // enable digital I/O on Port F
}

int main() {		
	PLL_Init();                       // 80 MHz system clock
	DAC_Init2();
	SysTick_Init(80);                 // 1 us SysTick periodic interrupts 
	EdgeInterrupts_Init();            // Initialize all available edge interrupts 
	Ultrasonic1_Init();               // Intitialize HC-SR04 ultrasonic sensor
	Buttons_Init();
	OmniControl_Init();
	while(1) {
		//WaitForInterrupt();
		Ultrasonic1_sendTrigger();
		Delay(500);
	}	 
}

void DAC_Init2(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate port D
	delay = SYSCTL_RCGC2_R; // allow time to finish activating
	GPIO_PORTD_AMSEL_R &= ~0x0F; // no analog
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // regular GPIO function
	GPIO_PORTD_DIR_R |= 0x0F; // make PD-0 out
	GPIO_PORTD_AFSEL_R &= ~0x0F;// disable alt funct on PD2-0
	GPIO_PORTD_DEN_R |= 0x0F;// enable digital I/O on PD2-0
}





