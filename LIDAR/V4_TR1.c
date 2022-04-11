#include "LPC17xx.h"                   // Device header
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include <math.h>

#define Pi 3.1415927

extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Init_PWM (void);
void Init_UART(void);

char start = 0xA5;
char start_scan = 0x20;
char stop_scan = 0x25;

int main(void){
	int i,j, nbTours=0,cmp=0 ;
	char  data[3000], trame[10], qualite[400], angle[1];
  float f_angle[400], f_distance[400] , f_mesure_cinq_deg[72];
	int positionX=1, positionY=1, valeur = 7;
  float 	pos_affichX =0, pos_affichY=0;
	int data_utile[72];
	
	
	Init_PWM();  //initialisation d'un signal PWM (25 Hz à rapport cyclique 41%)
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_6x8);
	
 /*
//-------- <<< Recuperation donnéee >>> ------------------
*/
	while(1)
	{
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&start,1);
	
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&start_scan,1);
		
	Driver_USART1.Receive(data,2200);
	while (Driver_USART1.GetRxCount()<2200);  // on attend que 1 case soit plein
	
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&start,1);
	
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(&stop_scan,1);
		
		for (i=0; i<100000;i++);

		
	

 /*
//-------- <<< Traitement data >>> ------------------
*/
						
// Boucle récupération angle						
						for(i=0;i<400;i++)
			{
				f_angle[i]=((data[8+i*5]&0xFE) + data[9+i*5]*128); // on retire bit de check
			f_angle[i] =  f_angle[i]/64;              // valeur d'angle codée sur 15 bits
//				sprintf(trame,"%d",(int) f_angle[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=50;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>300)break; 
		}

// Boucle récupération distance
					for(i=0;i<400;i++)
			{
				f_distance[i]=(data[10+i*5] + data[11+i*5]*256);
				f_distance[i] =  f_distance[i]/65535 *6*2 ;
//
			}		
			
			
 // Boucle qualité
				for(i=0;i<400;i++)
			{
			qualite[i]=(data[7+i*5]);
//						sprintf(trame,"%02x", qualite[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=20;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>300)break; 
//				
//				
//				
//				if ( (qualite[i] & 0x01 ) == 0x01) nbTours++;	
//				if ( qualite[i]  == 0x02) cmp++;	 

			}			
//			sprintf(trame,"%d" ,nbTours);
//				GLCD_DrawString(1,1,trame);
//				sprintf(trame,"%d" ,cmp);
//				GLCD_DrawString(20,20,trame);
//			
	
		// Boucle traitement
		
			
	for (i=0; i<72; i++)
			{
				for (j=0; j<400; j++)
				{
					if ( (f_angle[j]> (5*i)) && (f_angle[j]< (5*i+5)))
					{
						cmp ++;
						f_mesure_cinq_deg[i]= (f_distance[j] + f_mesure_cinq_deg[i])/ cmp;
								
					}		
				}
				cmp =0;
			}
			
// Affichage angle et distance 
			
//	for (i=0; i<72; i++){

//		sprintf(trame," %0.3f" , f_mesure_cinq_deg[i]  );
//	GLCD_DrawString(positionX,positionY,trame);
//				positionX+=50;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>400)break; 
//		
//	}		
			
	  GLCD_ClearScreen();		
	for (i=0; i<72; i++)
	{
		if (f_mesure_cinq_deg[i] > 2) 
		{
		   pos_affichX = 160 + 110*cosf(i*0.0872664);
			pos_affichY= 120 + 110*sinf(i*0.0872664);
		}
			
		else 
		{
				pos_affichX = 160 + 55 *f_mesure_cinq_deg[i] * cosf(i*0.0872664);
			pos_affichY= 120 + 55 *f_mesure_cinq_deg[i] * sinf(i*0.0872664);
		
		}		
//GLCD_DrawPixel((int)pos_affichX, (int)pos_affichY);
	
		GLCD_DrawRectangle (pos_affichX, pos_affichY,3, 3);
		
//		
//		sprintf(trame,"%0.1f : %0.1f ", pos_affichX, pos_affichY );
//	GLCD_DrawString(positionX,positionY,trame);
//				positionX+=80;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>400)break; 
//		
		}	
for(i=0;i<3000;i++)
{
	data[i]=0;
}	
for(i=0;i<400;i++)
{
	qualite[i]=0;
	f_angle[i]=0;
	f_distance[i]=0;	
}	
for (i=0; i<72; i++)
	{
		f_mesure_cinq_deg[i]=0;
	}
cmp=0;
	
		
	

	
	for (i=0; i<10000000;i++);
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
