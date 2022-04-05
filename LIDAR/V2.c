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

char start = 0xA5;
char start_scan = 0x20;

int main(void){
	int i,j, nbTours=0;
	char  data[2000], trame[1];
  float f_angle[300], f_distance[300];
	int positionX=1, positionY=1, valeur = 7;
	int data_utile[72];
	
	Init_PWM();  //initialisation d'un signal PWM (25 Hz à rapport cyclique 41%)
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&start,1);
	
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&start_scan,1);
		
	Driver_USART1.Receive(data,1000);
	while (Driver_USART1.GetRxCount()<1000);  // on attend que 1 case soit plein


//		for( i=0; i<7; i++){
//			sprintf(trame,"%02x",data[i]);
//			GLCD_DrawString(positionX, positionY,trame);
//			positionY+=30;
//		}

//			for ( j=0; j<8; j++){
//				positionX += 35;
//				positionY = 1;
//						for( i=0; i<5; i++){
//							sprintf(trame,"%02x",data[valeur]);
//							GLCD_DrawString(positionX, positionY,trame);
//							positionY+=30;
//							valeur++;
//								
//			}
//}
						
// Boucle récupération angle						
						for(i=0;i<30;i++)
			{
				f_angle[i]=((data[8+i*5]&0xFE) + data[9+i*5]*128); // on retire bit de check
				f_angle[i] =  f_angle[i]/61439 * 360;              // valeur d'angle codée sur 15 bits
//				sprintf(trame,"%d",(int) f_angle[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=50;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=30;
//				}
//				if (positionY>300)break; 
		}

// Boucle récupération distance
					for(i=0;i<30;i++)
			{
				f_distance[i]=(data[10+i*5] + data[11+i*5]*256);
				f_distance[i] =  f_distance[i]/65535 * 16;
//				sprintf(trame,"%0.2f" ,f_distance[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=90;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=30;
//				}
//				if (positionY>400)break; 
			}		
			
			
// Boucle nombre de tours	

					for(i=0;i<199;i++)
{
if ( (data[7+ 5*i]%2 ) ==1) nbTours++;	
	
}
sprintf(trame,"%d" ,nbTours);
				GLCD_DrawString(positionX,positionY,trame);

// Boucle traitement angle 5°
			
		       for(i=0;i<72;i++)
		{
		
		
		
		}
			
			

return 0;
	}
  
	



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

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
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
