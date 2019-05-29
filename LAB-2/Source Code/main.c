// main.c
// Runs on LM4F120 or TM4C123
// C2_Toggle_PF1, toggles PF1 (red LED) at 5 Hz
// Daniel Valvano, Jonathan Valvano, and Ramesh Yerraballi
// January 18, 2016

// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define PF4                     (*((volatile unsigned long *)0x40025040))
#define PF3                     (*((volatile unsigned long *)0x40025020))
#define PF2                     (*((volatile unsigned long *)0x40025010))
#define PF1                     (*((volatile unsigned long *)0x40025008))
#define PF0                     (*((volatile unsigned long *)0x40025004))
#define GPIO_PORTF_DR2R_R       (*((volatile unsigned long *)0x40025500))
#define GPIO_PORTF_DR4R_R       (*((volatile unsigned long *)0x40025504))
#define GPIO_PORTF_DR8R_R       (*((volatile unsigned long *)0x40025508))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

//Global variables
unsigned long SW1, SW2;
int pressed_1 = 0;
int pressed_2 = 0;

void FlashSOS(void);
void PortF_Init(void);
void Delay(double sec);
void RedBlueGreen(void);
void SW2_Pressed(void);

int main(void){  
  PortF_Init();  // make PF1 out (PF1 built-in LED)
	while(1){                  
		SW1 = GPIO_PORTF_DATA_R&0x10;
		SW2 = GPIO_PORTF_DATA_R&0x01;
		if(SW1 == 0x10){
			RedBlueGreen();
		}
		if(SW2 == 0x00){ //IF SW2 is pressed
			SW2_Pressed();
		}
		if(SW1 == 0x00) { // If SW1 is pressed
			FlashSOS();
		}	
	}
}

void Delay(double sec){
	unsigned long volatile time;
  time = 371192*sec;  // When sec==1, it represents 1 sec
  while(time){
		time--;
		SW1 = GPIO_PORTF_DATA_R&0x10;
		SW2 = GPIO_PORTF_DATA_R&0x01;
		if(SW1 == 0x00){
			pressed_1 = 1;
		}
		if(SW2 == 0x00){
			pressed_2 = 1;
		}
  }
}

void RedBlueGreen(void){
	GPIO_PORTF_DATA_R |= 0x02;  Delay(0.5);	// Red LED is on
	GPIO_PORTF_DATA_R &= ~0x02; 					  // Red LED is off
	GPIO_PORTF_DATA_R |= 0x04;  Delay(1);   // Blue LED is on
	GPIO_PORTF_DATA_R &= ~0x04; 					  // Blue LED is off
	GPIO_PORTF_DATA_R |= 0x08;  Delay(1.5); // Green LED is on
	GPIO_PORTF_DATA_R &= ~0x08;  					  // Green LED is off
	if(pressed_1 == 1){
		FlashSOS();
	}
	if(pressed_2 == 1){
		SW2_Pressed();
	}
}

void SW2_Pressed(void){
	pressed_2 = 0;
	GPIO_PORTF_DATA_R |= 0x0C;  Delay(2); // Sky blue LED is on
	GPIO_PORTF_DATA_R &= ~0x0C;
	if(pressed_1 == 1){
		FlashSOS();
	}
	if(pressed_2 == 1){
		SW2_Pressed();
	}
}

void FlashSOS(void){
	pressed_1 = 0;
	//S
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25);
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25);
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25);
  //O
  GPIO_PORTF_DATA_R |= 0x0E;  Delay(0.5);
  GPIO_PORTF_DATA_R &= ~0x0E; Delay(0.5);
  GPIO_PORTF_DATA_R |= 0x0E;  Delay(0.5);
  GPIO_PORTF_DATA_R &= ~0x0E; Delay(0.5);
  GPIO_PORTF_DATA_R |= 0x0E;  Delay(0.5);
  GPIO_PORTF_DATA_R &= ~0x0E; Delay(0.5);
  //S
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25);
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25);
  GPIO_PORTF_DATA_R |= 0x02;  Delay(0.25);
  GPIO_PORTF_DATA_R &= ~0x02; Delay(0.25); 
	if(pressed_2 == 1){
		SW2_Pressed();
	}
	if(pressed_1){
		FlashSOS();
	}
}

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
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
