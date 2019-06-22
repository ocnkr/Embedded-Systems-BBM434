#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "Lab14_MeasurementOfDistance/ADC.h"

void EnableInterrupts(void);
void WaitForInterrupt(void);
void DAC_Out(unsigned long data);
void LED_Init(void);
void Sound_Init(void);
void SysTick_Init(unsigned long count);
void SysTick_Handler(void);
void DAC_Init(void);

unsigned long ADCvalue1;
unsigned long ADCvalue2;
unsigned int count = 0;
unsigned int period = 20;
unsigned int i = 0;
unsigned int angle1 = 0;
unsigned int angle2 = 0;
unsigned int max_open_leds = 0;
unsigned int brightness = 0;
unsigned int on_percentage = 0;
unsigned long duty_cycle = 0;
unsigned long leds[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};

const unsigned char SineWave[32] = {8,9,10,12,13,14,14,15,15,15,14,14,13,12,10,9,8,6,5,3,2,1,1,0,0,0,1,1,2,3,5,6};
unsigned char Index=0; // Index varies from 0 to 31

unsigned int DO = 1908;
unsigned int RE = 1700;
unsigned int MI = 1515;
unsigned int FA = 1432;
unsigned int SOL = 1275;
unsigned int LA = 1136;
unsigned int SI = 1012;
unsigned int DO_ = 956;


int main(void) {
	
	ADC0_Init();
	LED_Init();
	Sound_Init();
	
	while(1){
		WaitForInterrupt();
	}
}

/*
 * PortB is used to connect LEDs.
 * PortB5-0
*/
void LED_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; 
	delay = SYSCTL_RCGC2_R;	
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTB_AMSEL_R &= ~0x3F; 
	GPIO_PORTB_DIR_R |= 0x3F;	
	GPIO_PORTB_AFSEL_R &= ~0x3F; 
	GPIO_PORTB_DEN_R |= 0x3F; 
}

/*
 * Finds ADC value and calculates angle according to it.
 * Reloads systick value according to notes.
 * Calculates duty cycle and pwm.
*/
void SysTick_Handler(void) {
	count++;
	ADCvalue1 = ADC0_In(); // ADC value for sound
	Index = (Index+1)&0x1F; // index goes through 0 to 31 
  DAC_Out(SineWave[Index]); // output one value each interrupt
	angle1 = (ADCvalue1 * 270) / 4095;

	if(angle1 <= 34){  // DO	
		NVIC_ST_RELOAD_R = DO-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 68){  //RE
		NVIC_ST_RELOAD_R = RE-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 102){  //MI
		NVIC_ST_RELOAD_R = MI-1;
		NVIC_ST_CTRL_R = 0x0007;
	}
	else if(angle1 <= 136){  //FA
		NVIC_ST_RELOAD_R = FA-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 170){  //SOL
		NVIC_ST_RELOAD_R = SOL-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 204){  //LA
		NVIC_ST_RELOAD_R = LA-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 238){  //SI
		NVIC_ST_RELOAD_R = SI-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	else if(angle1 <= 270){  //DO
		NVIC_ST_RELOAD_R = DO_-1;
		NVIC_ST_CTRL_R = 0x0007;	
	}
	if(count == period) {
		ADCvalue2 = ADC0_In();
		
		angle2 = (ADCvalue2 * 270) / 4095;
		max_open_leds = (angle2 > 265) ? 6 : angle2 / 45;
		brightness = (angle2 % 45);
		
		// Duration of HIGH
		duty_cycle = (brightness * period) / 45;
		count = 0;
	}

	if(duty_cycle > count) { // Duration of HIGH
		for(i = 0; i < max_open_leds; i++) {
			GPIO_PORTB_DATA_R |= leds[i];   // Turn on LEDs
		}
	} else {  // Duration of LOW
		GPIO_PORTB_DATA_R &= ~leds[max_open_leds - 1];   // Turn off LEDs
	}
}

// **************DAC_Init*********************
// Initialize 3-bit DAC
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate port D
	delay = SYSCTL_RCGC2_R; // allow time to finish activating
	GPIO_PORTD_AMSEL_R &= ~0x0F; // no analog
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // regular GPIO function
	GPIO_PORTD_DIR_R |= 0x0F; // make PD-0 out
	GPIO_PORTD_AFSEL_R &= ~0x0F;// disable alt funct on PD2-0
	GPIO_PORTD_DEN_R |= 0x0F;// enable digital I/O on PD2-0
}

// **************Sound_Init*********************
// Initialize Systick countic interrupts
// Input: interrupt count
//        Units of count are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void Sound_Init(void){
  DAC_Init();  // Port D is DAC
  Index = 0;
	NVIC_ST_RELOAD_R = 2-1;// reload value
  NVIC_ST_CTRL_R = 0; // disable SysTick during setup
  NVIC_ST_CURRENT_R = 0; // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
}

// **************DAC_Out*********************
// output to DAC
// Input: 3-bit data, 0 to 7
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTD_DATA_R = data;
}
