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
#define GPIO_PORTE_DATA_BITS_R  ((volatile unsigned long *)0x40024000)
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_IS_R         (*((volatile unsigned long *)0x40024404))
#define GPIO_PORTE_IBE_R        (*((volatile unsigned long *)0x40024408))
#define GPIO_PORTE_IEV_R        (*((volatile unsigned long *)0x4002440C))
#define GPIO_PORTE_IM_R         (*((volatile unsigned long *)0x40024410))
#define GPIO_PORTE_RIS_R        (*((volatile unsigned long *)0x40024414))
#define GPIO_PORTE_MIS_R        (*((volatile unsigned long *)0x40024418))
#define GPIO_PORTE_ICR_R        (*((volatile unsigned long *)0x4002441C))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_ST_CTRL_R      		(*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    		(*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   		(*((volatile unsigned long *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))  
#define NVIC_PRI1_R             (*((volatile unsigned long *)0xE000E41C)) 

//Global variables
unsigned long SW;
unsigned long color_arr[] = {0x02, 0x04, 0x08};
int pressed = 0;
int indx = -1;

//Function prototypes
void PortE_Init(void);
void SysTick_Init(unsigned long period);
void GPIOPortE_Handler(void);
void SysTick_Handler(unsigned int delay);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

int main(void){ 
	DisableInterrupts();
	PortE_Init();
	SysTick_Init(8000000);
	EnableInterrupts();   // Enable global Interrupt flag
	while(1){
		WaitForInterrupt();
	}
}

void SysTick_Handler(unsigned int delay){
	if(!pressed){
		indx = (indx + 1) % 3;
	}
	else {
		indx = ((indx - 1) + 3) % 3;
	}
	GPIO_PORTE_DATA_R = color_arr[indx]; 
}

void GPIOPortE_Handler(void){ 	// button control
	if(GPIO_PORTE_RIS_R & 0x01){  // PE0 touch
    GPIO_PORTE_ICR_R = 0x01;  	// acknowledge flag0
		pressed = !pressed;					// pressed is toggled
  }
}

void PortE_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     		// activate clock for Port E
	delay = SYSCTL_RCGC2_R;           		// allow time for clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   		// unlock GPIO Port E
	GPIO_PORTE_CR_R = 0x0E;           		// allow changes to PE1-3
	GPIO_PORTE_AMSEL_R = 0x00;        		// disable analog on PE
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF; 		// configure PE3-0 as GPIO
	GPIO_PORTE_DIR_R = 0x0E;          		// PE0 in, PF3-1 out
	GPIO_PORTE_AFSEL_R = 0x00;        		// disable alt funct on PE7-0
	GPIO_PORTE_DEN_R = 0x1F;          		// enable digital I/O on PE4-0
	GPIO_PORTE_IS_R &= ~0x01;     				// PE0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x01;    				// PE0 is not both edges
  GPIO_PORTE_IEV_R &= ~0x01;    				// PE0 falling edge event
  GPIO_PORTE_ICR_R = 0x01;      				// clear flag0
  GPIO_PORTE_IM_R |= 0x01;      				// arm interrupt on PE0
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000020; // priority 1
  NVIC_EN0_R = 0x00000010;      				// enable interrupt 30 in NVIC
}

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;							// disable SysTick during setup
	NVIC_ST_RELOAD_R = period - 1;	// reload value
	NVIC_ST_CURRENT_R = 0;					// any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;
  NVIC_ST_CTRL_R = 0x00000007;
}

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06
