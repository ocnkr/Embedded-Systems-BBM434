#include "OmniControl.h"
#include "Timers.h"    
#include "SysTick.h" 
#include "Ultrasonic.h" 

extern ultrasonic_t ult1;


/** @brief  Timer tasks to be called in timer handlers
  * @input  timerNum: Determines which timer function to be selected
  * @output None
  */
void Timer_Task(unsigned long timerNum)
{}

void Timer0_Task(void){Timer_Task(0);}
void Timer1_Task(void){Timer_Task(1);}
void Timer2_Task(void){Timer_Task(2);}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 31
// Output: none
void DAC_Out(unsigned long data){
	GPIO_PORTD_DATA_R = data;
} 

void Timer3_Task(void)	
{ 
	if(ult1.play == 1) {
		if((GPIO_PORTE_DATA_R & 0x20) == 0x20){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x04;
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt
			TIMER3_TAILR_R = DO2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001;  
		}
		else if((GPIO_PORTE_DATA_R & 0x02) == 0x02){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x0A;
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt
			TIMER3_TAILR_R = RE2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001;  
		}
		else if((GPIO_PORTE_DATA_R & 0x04) == 0x04){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x02;
			Index2 = (Index2+1)&0x1F;  // index goes through 0 to 31 
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt
			TIMER3_TAILR_R = MI2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001;  
		}
		else if((GPIO_PORTE_DATA_R & 0x08) == 0x08){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x06;
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt	
			TIMER3_TAILR_R = FA2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001; 
		}
		else if((GPIO_PORTE_DATA_R & 0x10) == 0x10){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x0C; 
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt	
			TIMER3_TAILR_R = SOL2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001;  
		}
		else if((GPIO_PORTB_DATA_R & 0x08) == 0x08){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x0E; 
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt	
			TIMER3_TAILR_R = LA2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001; 
		}
		else if((GPIO_PORTB_DATA_R & 0x40) == 0x40){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x02;
			Index2 = (Index2+1)&0x1F;				
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt
			TIMER3_TAILR_R = SI2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001; 
		}
		else if((GPIO_PORTB_DATA_R & 0x80) == 0x80){
			ult1.play = 1;
			GPIO_PORTF_DATA_R = 0x02; 
			Index2 = (Index2+1)&0x1F;
			DAC_Out(SineWave2_High[Index2]);  // output one value each interrupt
			TIMER3_TAILR_R = DO_2-1;    // 4) reload value
			TIMER3_CTL_R = 0x00000001; 
		}
		else{
			GPIO_PORTF_DATA_R = 0x00; 
			Index2 = (Index2+1)&0x1F;  // index goes through 0 to 255
			DAC_Out(SineWave2_Low[Index2]);
			ult1.play = 0;
			TIMER3_TAILR_R = SI2;    // 4) reload value
			TIMER3_CTL_R = 0x00000001;
		}
	}
}

void Timer4_Task(void)	
{}

/** @brief  Timer initializations for PID loops
  * @input  None
  * @output None
  */
void OmniControl_Init(void)
{
	Timer0_Init(&Timer0_Task, 80000);      // timer period: 1ms
  Timer1_Init(&Timer1_Task, 80000);      // timer period: 1ms
	Timer2_Init(&Timer2_Task, 80000);      // timer period: 1ms
	Timer3_Init(&Timer3_Task, 80000);      // timer period: 10ms
	Timer4_Init(&Timer4_Task, 80000);    // timer period: 6ms
}
