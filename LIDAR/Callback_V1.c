*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "LPC17xx.h"                   // Device header
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"



extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Init_PWM (void);
void Init_UART(void);
void UARTthreadT(void const * argument);
void UARTthreadR(void const * argument);



/*
 * main: initialize and start the system
 */
 
 

 void UARTcallback(uint32_t event);
 
 osThreadId ID_UARTthreadT ;
 osThreadDef (UARTthreadT,osPriorityNormal,1,0);
 
  
 osThreadId ID_UARTthreadR ;
 osThreadDef (UARTthreadR,osPriorityNormal,1,0);

int main (void) {
  
	
	Init_PWM(); //initialisation d'un signal PWM (25 Hz à rapport cyclique 41%)  
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_DrawString(1,1,"start");
		
osKernelInitialize ();                    // initialize CMSIS-RTOS
  // initialize peripherals here
ID_UARTthreadT = osThreadCreate( osThread ( UARTthreadT ), NULL ) ;
ID_UARTthreadR = osThreadCreate( osThread ( UARTthreadR ), NULL ) ;
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}


// FONCTION TRANSMISSION UART LIDAR //
void UARTthreadT(void const * argument)
	{
			char start = 0xA5;
char start_scan = 0x20;
		while(1)
			{
				//while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
				Driver_USART1.Send(&start,1);
				osSignalWait(0x01, osWaitForever);		
	
			//while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
			  Driver_USART1.Send(&start_scan,1);
				GLCD_DrawString(1,1,"essai 1");
				osSignalWait(0x01, osWaitForever);
				GLCD_DrawString(1,1,"essai 2");
				osDelay(osWaitForever);
	}
}
// FONCTION RECEPTION UART LIDAR //
void UARTthreadR(void const * argument)
	{
		osEvent evt;
	int i,j;
	char  data[10000], trame[1], nouveauTour[2];
	int positionX=1, positionY=1, valeur = 7;

		while(1)
			{
				evt = osSignalWait(0x01, osWaitForever);// sommeil sur attente réception
				
				GLCD_DrawString(1,1,"oui");
				osDelay(1000);
				Driver_USART1.Receive(data,1000);
	//while (Driver_USART1.GetRxCount()<10000);  // on attend que 1 case soit plein


		for( i=0; i<1000; i++){
			sprintf(trame,"%02x",data[i]);
			GLCD_DrawString(1,1,trame);
		
		}
}
			}

// FONCTION CALLBACK //		
void UARTcallback (uint32_t event)// arguments imposés
	{
	switch (event)
	{
		case ARM_USART_EVENT_SEND_COMPLETE :
		osSignalSet(ID_UARTthreadT, 0x01);
	break;
		
		case ARM_USART_EVENT_RECEIVE_COMPLETE :
		osSignalSet(ID_UARTthreadR, 0x01);
	break;
	}
	}
	
 // FONCTION INIT PWM //
void Init_PWM (void)
{
	LPC_SC->PCONP = LPC_SC->PCONP | (1<<6); //active le périphérique PWM1
	LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4 | (1<<4); //P2.2 en mode PWM1.3
	LPC_PWM1->CTCR = 0; //on compte des périodes d'horloge
	LPC_PWM1->PR = 0; //Prescaler à 0
	LPC_PWM1->MR0 = 1000-1; //valeur max du comptage pour période PWM
	LPC_PWM1->MR3 = 600; //pour le rapport cyclique de 25% sur la sortie 3
	LPC_PWM1->MCR = LPC_PWM1->MCR | (1<<1); //RAZ du compteur si correspondance avec MR0
	LPC_PWM1->PCR = LPC_PWM1->PCR | (1<<11); //active la sortie PWM1.3
	LPC_PWM1->TCR = 1; //démarre le comptage du Timer
}

// FONCTION INIT UART //
void Init_UART(void){
	Driver_USART1.Initialize(UARTcallback);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS | 
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}
