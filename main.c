/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C

#include "Board_LED.h"                  // ::Board Support:LED

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif



#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates

  * @{
  */

/* Par moi -----------------------------------------------------------*/
// #define add_capt1 = 0xe6; Pourquoi cela ne marche-t-il pas ?


extern ARM_DRIVER_I2C Driver_I2C1;// “déclaration” structure I2C0
void Thread_envoi_I2C (void const * argument);
void Init_I2C(void);
void callback_I2C (uint32_t event);
void premier_envoi_I2C(void);



osThreadId ID_Envoi_I2C ;
osThreadDef (Thread_envoi_I2C, osPriorityNormal, 1, 0);

static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
void Init_I2C(void)
{
	Driver_I2C1.Initialize(callback_I2C);  // début initialisation
	Driver_I2C1.PowerControl(ARM_POWER_FULL); // alimentation périphérique
	Driver_I2C1.Control(ARM_I2C_BUS_SPEED, // 2nd argument = débit
	ARM_I2C_BUS_SPEED_STANDARD  );    // =100 kHz
	//Driver_I2C1.Control(ARM_I2C_BUS_CLEAR,0 );//Attention boucle bloquante
}

void callback_I2C (uint32_t event){ //Fonction lancee automatiquement pour mise à 1 de l'event de ID_Envoi_I2C
	
	static volatile uint32_t I2C_Event;
	
	// Sauvegarde les events recus
  I2C_Event |= event;
	if (event & ARM_I2C_EVENT_TRANSFER_DONE ) {
		osSignalSet(ID_Envoi_I2C, 0x01); //Mise à 1 de l'event 1
  }
}

void premier_envoi_I2C(void){
	unsigned char adressecapt1;
	uint8_t ecriture_reg[2], lecture_reg2[1], lecture_reg3[1];
	unsigned char tabDATA2[1],tabDATA3[1];
	
	
  ecriture_reg[0]=0;//Registre 0
  ecriture_reg[1]=0x51;
	lecture_reg2[0]=2; //Registre 2 pour lire la mesure des 8 bits de poids fort
	lecture_reg3[0]=3; //Registre 3 pour lire la mesure des 8 bits de poids faible
	adressecapt1 = 0xe6 ;
  adressecapt1 = (adressecapt1>>1); //0x73
		
    //Pour initier la mesure
		Driver_I2C1.MasterTransmit(adressecapt1, ecriture_reg, 2, false);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		
		//Pour recevoir la premiere mesure
		osDelay(100);
		Driver_I2C1.MasterTransmit(adressecapt1, lecture_reg2, 1, true);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		Driver_I2C1.MasterReceive(adressecapt1, (uint8_t*)tabDATA2,1, false); //Si renvoi en octets (8bits)
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		
		
		//Pour recevoir la deuxieme mesure
		Driver_I2C1.MasterTransmit(adressecapt1, lecture_reg3, 1, true);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		Driver_I2C1.MasterReceive(adressecapt1, (uint8_t*)tabDATA3, 1, false); //Si renvoi en octets (8bits)
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
}
	
void Thread_envoi_I2C (void const * argument){
	unsigned char adressecapt1;
	uint8_t ecriture_reg[2], lecture_reg2[1], lecture_reg3[1];
	unsigned char tabDATA2[1],tabDATA3[1];
	unsigned short donneecapt1;
	
	
  ecriture_reg[0]=0;//Registre 0
  ecriture_reg[1]=0x51;
	lecture_reg2[0]=2; //Registre 2 pour lire la mesure des 8 bits de poids fort
	lecture_reg3[0]=3; //Registre 3 pour lire la mesure des 8 bits de poids faible
	adressecapt1 = 0xe6 ;
  adressecapt1 = (adressecapt1>>1); //0x73
	premier_envoi_I2C();
	while(1){
			
		LED_On (1);
		
    //Pour initier la mesure
		Driver_I2C1.MasterTransmit(adressecapt1, ecriture_reg, 2, false);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		
		LED_On (2);
		
		//Pour recevoir la premiere mesure
		osDelay(100);
		Driver_I2C1.MasterTransmit(adressecapt1, lecture_reg2, 1, true);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		Driver_I2C1.MasterReceive(adressecapt1, (uint8_t*)tabDATA2, 1, false); //Si renvoi en octets (8bits)
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		
		
		//Pour recevoir la deuxieme mesure
		Driver_I2C1.MasterTransmit(adressecapt1, lecture_reg3, 1, true);
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		Driver_I2C1.MasterReceive(adressecapt1, (uint8_t*)tabDATA3, 1, false); //Si renvoi en octets (8bits)
		//while (Driver_I2C1.GetStatus().busy==1); //Attente de la fin de la transmission
		osSignalWait(0x01, osWaitForever);
		
		
		LED_On (3);
		
		//Si renvoi en 2 octets (16 bits)
		donneecapt1=((tabDATA2[0]<<8)|tabDATA3[0]); //Donnee finale sur 16 bits du resultat de la distance en cm
	}
}
int main(void)
{
  HAL_Init();
	Init_I2C();
	LED_Initialize ();
	SystemClock_Config();
  SystemCoreClockUpdate();
	osKernelInitialize ();
  /* Configure the system clock to 168 MHz */
  
	
	//#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
	
  
	ID_Envoi_I2C= osThreadCreate(osThread(Thread_envoi_I2C),NULL);
	
	//NVIC_SetPriority(USART2_IRQn,2);		// nécessaire ? (si LCD ?)
	
	

  osKernelStart();
	osDelay(osWaitForever);
	
 
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

