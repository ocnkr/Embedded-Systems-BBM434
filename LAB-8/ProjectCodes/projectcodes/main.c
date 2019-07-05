
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "LED.h"
#include "Lab15_SpaceInvaders/Nokia5110.c"

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


char Fetch[] = "GET /apps/thinghttp/send_request?api_key=IAVBJSQREFNWD3E3\r\nHost:api.thingspeak.com\r\n\r\n";
char time[6];
int i;

int main(void){  
  DisableInterrupts();
	Nokia5110_Init();
	Nokia5110_Clear();
	Nokia5110_SetCursor(0,2);
	Nokia5110_OutString("Connecting..");
  PLL_Init(Bus80MHz);
  LED_Init();  
  Output_Init();       // UART0 only used for debugging
  ESP8266_Init(115200);      // connect to access point, set up as client
  ESP8266_GetVersionNumber();
  while(1){
		Nokia5110_Clear();
		Nokia5110_SetCursor(1,2);
		Nokia5110_OutString("Loading...");
    ESP8266_GetStatus();
    if(ESP8266_MakeTCPConnection("api.thingspeak.com")){ // open socket in server
      LED_GreenOn();
      ESP8266_SendTCP(Fetch);  //get data from the website
			for(i = 2; i < 7; i++){
				time[i-2] = ServerResponseBuffer[i]; //store data in an array
			}
			time[5] = '\0';
    }
		
		Nokia5110_DisplayBuffer();
		Nokia5110_SetCursor(0,0);
		Nokia5110_OutString("Time:");
		Nokia5110_SetCursor(0,1);
		Nokia5110_OutString(time);  //print data on Nokia5110 LCD
    
		ESP8266_CloseTCPConnection();
    while(Board_Input()==0){// wait for touch
    }; 
    LED_GreenOff();
    LED_RedToggle();
  }
}




