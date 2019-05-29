// main.c
// Runs on LM4F120 or TM4C123
// C2_Toggle_PF1, toggles PF1 (red LED) at 5 Hz
// Daniel Valvano, Jonathan Valvano, and Ramesh Yerraballi
// January 18, 2016

//*****************************************************************************
// define GPIO registers (PORTE)
// define SYSCTL_RCGC2_R
// define variables for SysTick
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
#define GPIO_PORTE_DR2R_R       (*((volatile unsigned long *)0x40024500))
#define GPIO_PORTE_DR4R_R       (*((volatile unsigned long *)0x40024504))
#define GPIO_PORTE_DR8R_R       (*((volatile unsigned long *)0x40024508))
#define GPIO_PORTE_ODR_R        (*((volatile unsigned long *)0x4002450C))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_SLR_R        (*((volatile unsigned long *)0x40024518))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define GPIO_PORTE_ADCCTL_R     (*((volatile unsigned long *)0x40024530))
#define GPIO_PORTE_DMACTL_R     (*((volatile unsigned long *)0x40024534))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_ST_CTRL_R      		(*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    		(*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   		(*((volatile unsigned long *)0xE000E018))

//Global variables
unsigned long SW;
int pressed = 0;

//Function prototypes
void PortE_Init(void);
void SysTick_Init(void);
void RedGreenYellow_Wait(unsigned int delay);
void YellowGreenRed_Wait(unsigned int delay);

int main(void){ 
	PortE_Init();
	SysTick_Init();
	while(1){
		while(!pressed){
			GPIO_PORTE_DATA_R = 0x02;	// Red LED is on	
			RedGreenYellow_Wait(2);		// Wait and check if pressed
			GPIO_PORTE_DATA_R = 0x04;	// Greed LED is on	
			RedGreenYellow_Wait(2);   // Wait and check if pressed
			GPIO_PORTE_DATA_R = 0x08;	// Yellow LED is on
			RedGreenYellow_Wait(2);		// Wait and check if pressed
		}
		while(pressed){
			GPIO_PORTE_DATA_R = 0x08;	// Yellow LED is on
			YellowGreenRed_Wait(2);		// Wait and check if pressed		
			GPIO_PORTE_DATA_R = 0x04;	// Greed LED is on
			YellowGreenRed_Wait(2);		// Wait and check if pressed
			GPIO_PORTE_DATA_R = 0x02;	// Red LED is on
			YellowGreenRed_Wait(2);		// Wait and check if pressed
		}
	}
}

void RedGreenYellow_Wait(unsigned int delay){
	int i;
	for(i=0; i<delay*10; i++){	
		NVIC_ST_RELOAD_R = 800000-1;  // number of counts to wait
		NVIC_ST_CURRENT_R = 0;
		while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait untill count bit is 1
			SW = GPIO_PORTE_DATA_R & 0x01;
			if(SW == 0x00){  // check if pressed or not
				pressed = 1;
			}
		}
	}
}

void YellowGreenRed_Wait(unsigned int delay){
	int i;
	for(i=0; i<delay*10; i++){	
		NVIC_ST_RELOAD_R = 800000-1;  // number of counts to wait
		NVIC_ST_CURRENT_R = 0;
		while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait untill count bit is 1
			SW = GPIO_PORTE_DATA_R & 0x01;
			if(SW == 0x00){		// check if pressed or not
				pressed = 0;
			}
		}
	}
}

void PortE_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port E
	delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
	GPIO_PORTE_CR_R = 0x0E;           // allow changes to PE1-3
	GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
	GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE4-0
	GPIO_PORTE_DIR_R = 0x0E;          // 5) PE0 in, PF3-1 out
	GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE7-0
	GPIO_PORTE_PUR_R = 0x00;
	GPIO_PORTE_DEN_R = 0x1F;          // 7) enable digital I/O on PE4-0
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;
  NVIC_ST_CTRL_R = 0x00000005;
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
