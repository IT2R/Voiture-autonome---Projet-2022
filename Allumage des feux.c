/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   STM32F4xx HAL API Template project 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "Driver_SPI.h"                 // ::CMSIS Driver:I2C

#include "Board_LED.h"                  // ::Board Support:LED

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common


#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#define nbLed 16
#define seuil 2000

#define clignotantDroite 0x01
#define clignotantGauche 0x02

char Data_LED[4+nbLed*4+(2+(int)((nbLed-1)/16))];


void LED (void const* argument);
void Clignotant(void const* argument);


osThreadId ID_LED;
osThreadDef(LED, osPriorityNormal, 1, 0);

osThreadId ID_Clignotant;
osThreadDef(Clignotant, osPriorityNormal, 1, 0);

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


void remplirTabLED(char *tab, char rang, char nb_led, unsigned char intensitee, unsigned char red, unsigned char green , unsigned char blue);

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void Configure_GPIO(void);
static void configure_ADC2_Channel_1(void);


extern ARM_DRIVER_SPI Driver_SPI1;

void Init_LED_SPI(void){
	//LED_SPI.Initialize(mySPI_callback);
	Driver_SPI1.Initialize(NULL);
	Driver_SPI1.PowerControl(ARM_POWER_FULL);
	Driver_SPI1.Control(ARM_SPI_MODE_MASTER | 
											ARM_SPI_CPOL1_CPHA1 | 
//											ARM_SPI_MSB_LSB | 
											ARM_SPI_SS_MASTER_UNUSED |
											ARM_SPI_DATA_BITS(8), 1000000);
	Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}
//https://microcontrollerslab.com/adc-stm32f4-discovery-board-with-hal-adc-driver
ADC_HandleTypeDef myADC2Handle;


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void LED (void const* argument) {
	int i;
	uint32_t  Adc_value;
	char Adc_value_char[10];
	
	for (i=0;i<4;i++){
		Data_LED[i] = 0;
	}
		
	Data_LED[sizeof(Data_LED)/sizeof(Data_LED[0])-2] = 0;
	Data_LED[sizeof(Data_LED)/sizeof(Data_LED[0])-1] = 0;
	
	remplirTabLED(Data_LED,1,nbLed,2,0,0,0);
  Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
	while (1)
  {
		//osDelay(1000);
		Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
		
		HAL_ADC_Start(&myADC2Handle); // start A/D conversion
		for(i=0;i<10;i++)
		ITM_SendChar(Adc_value_char[i]);
		if(HAL_ADC_PollForConversion(&myADC2Handle, osWaitForever) == 0x00U) //conversion complete
		{
			Adc_value  = HAL_ADC_GetValue(&myADC2Handle);
			if ( Adc_value > seuil)
			{
				LED_On(0);
				remplirTabLED(Data_LED,1,4,2,0,0,0);
				remplirTabLED(Data_LED,9,4,2,0,0,0);

				Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
			}
			else 
			{
				LED_Off(0);
				remplirTabLED(Data_LED ,1,4,8,127,127,127);
				remplirTabLED(Data_LED ,9,4,8,127,127,127);
				Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
			}
				
			/*while(Driver_SPI1.GetStatus != 0)
			{
				ITM_SendChar( (char) Driver_SPI1.GetStatus);
				osDelay(100);
			}*/
			
			for(i=0;i<10;i++)
			ITM_SendChar(Adc_value_char[i]);
			//osDelay(500);
		}
		
  }
}

void Clignotant (void const * argument)
{
	//ARM_CAN_MSG_INFO                rx_msg_info;
	char nb_data, i;
//	osEvent event;
	
	LED_On(0);
	for (i=0;i<4;i++){
		Data_LED[i] = 0;
	}
		
	Data_LED[sizeof(Data_LED)/sizeof(Data_LED[0])-2] = 0;
	Data_LED[sizeof(Data_LED)/sizeof(Data_LED[0])-1] = 0;
	nb_data = clignotantGauche;
	

	while(1)
	{
		LED_On(1);
		//event = osSignalWait(0x0001,osWaitForever);
		//Driver_CAN2.MessageRead(0, &rx_msg_info, data_buf, 8);
		//nb_data = rx_msg_info.dlc
		if (nb_data == clignotantDroite)
		{
			remplirTabLED(Data_LED,7,6,4,0,0,127);
			osDelay(750);
			remplirTabLED(Data_LED,7,6,0,0,0,0);
			osDelay(750);
		}
		else if (nb_data == clignotantGauche)
		{
			remplirTabLED(Data_LED,5,4,4,127,0,0);
			Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
			osDelay(750);
			LED_On(2);
			remplirTabLED(Data_LED,5,4,4,0,0,0);
			Driver_SPI1.Send(Data_LED,sizeof(Data_LED)/sizeof(Data_LED[0]));
			osDelay(600);
			LED_On(3);
		}
	}
}

int main(void)
{
	/* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* Add your application code here
     */
	//#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();
	
	//NVIC_SetPriority(USART2_IRQn,2);		// nécessaire ? (si LCD ?)
	
	LED_Initialize ();

	configure_ADC2_Channel_1();
	Configure_GPIO();
  /* Create thread functions that start executing, 
  Example: osThreadNew(app_main, NULL, NULL); */
	ID_LED = osThreadCreate (osThread(LED), NULL);
	ID_Clignotant = osThreadCreate(osThread(Clignotant),NULL);

	Init_LED_SPI();
  /* Start thread execution */
  
	
	
	osKernelStart();
//#endif
/* Infinite loop */
	osDelay(osWaitForever);
			
}

void remplirTabLED(char *tab, char rang, char nb_led, unsigned char intensitee, unsigned char red, unsigned char green , unsigned char blue) {
	char i;
	for (i = 0; i < nb_led ;i++)
	{
			tab[(rang+i)*4]= (0xe0 | intensitee);
			tab[1+(rang+i)*4]= blue;
			tab[2+(rang+i)*4]= green;
			tab[3+(rang+i)*4]= red;
	}
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

void Configure_GPIO(void)
{
	 
	GPIO_InitTypeDef ADCpin; //create an instance of GPIO_InitTypeDef C struct
	__HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock to GPIOA
	ADCpin.Pin = GPIO_PIN_1; // Select pin PA0
	ADCpin.Mode = GPIO_MODE_ANALOG; // Select Analog Mode
	ADCpin.Pull = GPIO_NOPULL; // Disable internal pull-up or pull-down resistor
	HAL_GPIO_Init(GPIOA, &ADCpin); // initialize PA0 as analog input pin
}

void configure_ADC2_Channel_1(void)
{
	
	ADC_ChannelConfTypeDef Channel_AN1; // create an instance of ADC_ChannelConfTypeDef
	
	myADC2Handle.Instance = ADC2; // create an instance of ADC2
	
	
	__HAL_RCC_ADC2_CLK_ENABLE(); // enable clock to ADC2 module
	myADC2Handle.Init.Resolution = ADC_RESOLUTION_12B; // select 12-bit resolution 
	myADC2Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV; //select  single conversion as a end of conversion event
	myADC2Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // set digital output data right justified
	myADC2Handle.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8; 
	HAL_ADC_Init(&myADC2Handle); // initialize AD2 with myADC2Handle configuration settings
	
  /*select ADC2 channel */
	
	Channel_AN1.Channel = ADC_CHANNEL_1; // select analog channel 0
	Channel_AN1.Rank = 1; // set rank to 1
	Channel_AN1.SamplingTime = ADC_SAMPLETIME_15CYCLES; // set sampling time to 15 clock cycles
	HAL_ADC_ConfigChannel(&myADC2Handle, &Channel_AN1); // select channel_0 for ADC2 module. 
}




/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
