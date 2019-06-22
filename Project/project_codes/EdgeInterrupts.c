#include "tm4c123gh6pm.h"   
#include "EdgeInterrupts.h" 


/** @brief  Initialize all available edge interrupts 
  * @input  None
  * @output None
  */
void EdgeInterrupts_Init(void)
{	
	EdgeInterrupts_PE0_Init();
	EnableInterrupts();   // Clears the I bit 
}


/** @brief  PE0 interrupt initialization on both edges
  * @input  None
  * @output None
  */
void EdgeInterrupts_PE0_Init(void)
{
	unsigned long volatile delay;
	SYSCTL_RCGC2_R|=0x00000010;            // activate clock for port E
  delay=SYSCTL_RCGC2_R;                  // dummy delay
	GPIO_PORTE_DIR_R&=~0x01;               // make PE0 input
	GPIO_PORTE_AFSEL_R&=~0x01;             // disable alt funct on PE0
	GPIO_PORTE_DEN_R|=0x01;                // enable digital I/O on PE0   
	GPIO_PORTE_PCTL_R&=~0x0000000F;        // configure PE0 as GPIO
	GPIO_PORTE_AMSEL_R= 0;                 // disable analog functionality on PE
	GPIO_PORTE_PDR_R|=0x01;                // enable weak pull-down on PE0
	GPIO_PORTE_IS_R&=~0x01;                // PE0 is edge-sensitive
	GPIO_PORTE_IBE_R|=0x01;                // PE0 is both edges
	GPIO_PORTE_ICR_R|=0x01;                // clear flag0
	GPIO_PORTE_IM_R|=0x01;                 // arm interrupt on PE0
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000060; // priority 3
  NVIC_EN0_R|= 0x00000010;               // enable interrupt 4 in NVIC (GPIO PORTE: Interrupt number: 4)
}


void GPIOPortE_Handler(void)     
{
	GPIOPortE_UltrasonicTask();
}
