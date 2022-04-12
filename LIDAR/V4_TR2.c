#include "LPC17xx.h"                   // Device header
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
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
void Ligne(int x0, int y0, int x1, int y1);

char start = 0xA5;
char start_scan = 0x20;
char stop_scan = 0x25;

int main(void){
	int i,j, nbTours=0,cmp=0 , debutTrame;
	char  data[2200], trame[10], qualite[400], angle[1];
  float f_angle[400], f_distance[400] , f_mesure_cinq_deg[72];
	int positionX=1, positionY=50, valeur = 7;
  float 	pos_affichX =0, pos_affichY=0;
	int data_utile[72], fnnl=0, non_fnnl=0, fnnl_1;
	
	
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
		GLCD_ClearScreen();
		

for(i=0;i<50;i++)
{
 if ( (data[i] ==0x02	&& data[i+5] ==0x02 ) || 
	    (data[i] ==0x3e	&& data[i+5] ==0x02 ) ||
      (data[i] ==0x02	&& data[i+5] ==0x3e ) || 
	    (data[i] ==0x3e	&& data[i+5] ==0x3e )) 
   {
		 debutTrame=i;

    break;		 
	 }	
 }
	

// /*
////-------- <<< Traitement data >>> ------------------
//*/
//						
// Boucle récupération angle						
						for(i=0;i<400;i++)
			{
				f_angle[i]=((data[(debutTrame+1)+i*5]&0xFE) + data[(debutTrame+2)+i*5]*128); // on retire bit de check
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
			

//// Boucle récupération distance
					for(i=0;i<400;i++)
			{
				f_distance[i]=(data[(debutTrame+3)+i*5] + data[(debutTrame+4)+i*5]*256);
				f_distance[i] =  f_distance[i]/65535 *6*2 ;
//				sprintf(trame,"%0.2f",f_distance[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=50;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>300)break;

			}		
	
			
 // Boucle qualité
				for(i=0;i<400;i++)
			{
			qualite[i]=(data[(debutTrame)+i*5]);
//						sprintf(trame,"%02x", qualite[i]);
//				GLCD_DrawString(positionX,positionY,trame);
//				positionX+=20;
//				
//				if(positionX>=300)
//				{					positionX=0;
//					        positionY+=20;
//				}
//				if (positionY>300)break; 
			}	
				

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
	
			
	
	// Boucle affichage //			
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
	Ligne(160,120,(int) pos_affichX, (int) pos_affichY);

		
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

void Ligne(int x0, int y0, int x1, int y1)
{
	int dx = x0<x1 ? (x1-x0) : (x0-x1);
	int sx = x0<x1 ? 1 : -1;
	int dy = y0<y1 ? (y1-y0) : (y0-y1);
	int sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;

	while(1)
	{
		GLCD_DrawRectangle (x0,y0, 1,1);
		if ((x0==x1) && (y0==y1)) break;
		e2 = 2*err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}
