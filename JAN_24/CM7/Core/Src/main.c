/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  * Capstone team 42 2022/2023
  * Author: Joseph Peterson (joepetejp@gmail.com)
  * Date: March 13 2023
  *
  * Overview: The purpose of this code is to collect both optical spectrometry and
  * bioimpedance data concurrently. The timing of this code is driven by two timers.
  * The first timer runs at 50 Hz and controlls the data collection speed for the
  * entire system. Each time the 50 Hz timer triggers, it begins a data collection cycle
  * which includes triggering the VNA, triggering the Homamatsu Chip, collecting from the
  * H-chip via external ADC and saving that data to solid state memory.
  *
  * The second timer runs at 100KHz and is associated with the variable counter which
  * appears frequently in the main loop of the code.This timer exactly matches the
  * frequency of the clock that tells the H-chip how fast to send data. It is this faster
  * timer that controls the timing of the actual data collection cycle.
  *
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
// Global Variables
//*************************Boolean Values************************************************************************************************************************************************
bool optReady = false; // True if the H-chip is ready to send data
bool slowSPI = false;
bool sendData = false;

//*************************Data Buffers**************************************************************************************************************************************************
uint16_t spi_read_buf; // Temporarily holds data which is read via SPI from the ADC
uint16_t spi_destroy_buf; // Holds data that is unimportant to us

//*************************Data Collection Cycle Variables*******************************************************************************************************************************
int counter = 0; // Fast counter associated with timer 2, incremented in interrupt function
int slowCounter = 0; // Slow counter associated with timer 16, also incremented in IF
int seconds = 0; // Counts how many seconds of data we have collected
int collected = 0; // Counts how many data samples have been collected in given data cycle
int needed = 130; // Number of samples needed from H-chip in given cycle
int counterPrevious = 0; // Records previous value of counter

//*************************LED Control Variables*****************************************************************************************************************************************
//float calibration = [0.5, 0.25, 0.125, 0.05, 0.025, 0.0125, 0.005];
//float LED_Voltages = [2.0, 2.0, 2.0, 2.0, 2.0];
//int LED_Cal_Set = [0, 0, 0, 0, 0];
//float vSense_Array[] = [0, 0, 0, 0, 0];
//int LED_Voltage_Shootover = [0, 0, 0, 0, 0];
//float Vat50mA = 2.6;

//*************************States********************************************************************************************************************************************************
enum states{collect,wait,test,checkLED}state; //States which main loop switches between
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM16_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */
TIM_HandleTypeDef htim2;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t value[3];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */



  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */

/* USER CODE END Boot_Mode_Sequence_0 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
 /* timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }*/
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
//HAL_HSEM_FastTake(HSEM_ID_0);
///*Release HSEM in order to notify the CPU2(CM4)*/
//HAL_HSEM_Release(HSEM_ID_0,0);
///* wait until CPU2 wakes up from stop mode */
//timeout = 0xFFFF;
//while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
//if ( timeout < 0 )
//{
//Error_Handler();
//}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim16);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);



  TIM4->CCR1 = 5;
  TIM4->CCR2 = 5; // 5/10 50 percent duty cycle, becomes the clock for the H-chip
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint16_t writeBuf1 = 0b0000100001100100; // LED
  uint16_t writeBuf2 = 0b0000010001100100; // LED
  uint16_t writeBuf3 = 0b0000110001100100; // LED
  uint16_t writeBuf4 = 0b0000001001100100; // LED
  uint16_t writeBuf5 = 0b0000101001100100; // LED
  uint16_t writeBuf6 = 0b0000011001100100; // LED
  uint16_t ADCData[needed];
  char uart_buf[1000];
  int uart_buf_len;
  char num[6];

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  slowSPI = true;
  MX_SPI2_Init();
  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf1,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf2,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf3,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf4,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf5,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf6,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  HAL_ADC_Start_DMA(&hadc1, value, 3);

  HAL_Delay(1000);

  //char spi_read_buf[20];

  state = collect; //Initial state. Set to test to test out a block of code.
  while (1)
  {
	  switch(state){
	  	  case collect:

	  		    if (counter == 0) // set spec reset pin high to tell h-chip to send data
				{
					HAL_GPIO_WritePin(GPIOD, SPEC_RST_Pin,GPIO_PIN_SET); // Tells H-chip to send data
					HAL_GPIO_WritePin(GPIOB, VNA_TRIG_Pin,GPIO_PIN_SET); // Tells VNA to collect data
				}
	  		    else if (counter == 10) // set spec reset back low
				{
					HAL_GPIO_WritePin(GPIOD, SPEC_RST_Pin,GPIO_PIN_RESET); // Sends SPEC_RST back low
					optReady = true; // We found that it was more effective to hard code the timing of this rather than tying optReady to an external interrupt
				}
				else if (optReady && (counter > counterPrevious) && (collected < needed) && (counter > 14)) // wait for AD_SP from the h-chip then uses spi2 to read from adc
				{

					for(int i = 0; i < 12; i++) // A little jenky, delays so that timing is right for data collection from H-chip
					{
						HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);//This is the onboard red LED
					}
					HAL_GPIO_WritePin(GPIOA,ADC_CS_Pin,GPIO_PIN_SET); //CS for ADC, ADC collects at the moment this goes high
				    HAL_SPI_Receive(&hspi2, &spi_destroy_buf, 1, 100); //ADC needs this clock to convert analog value to Digital
				    HAL_GPIO_WritePin(GPIOA,ADC_CS_Pin,GPIO_PIN_RESET); //Tells ADC we are done sampling
				    HAL_SPI_Receive(&hspi2, &spi_read_buf, 1, 100); // Read digital value from ADC
				    counterPrevious = counter; // So we only collect when the value of counter changes
				    ADCData[collected] = spi_read_buf; // append to array
				    collected++;

				}
				else if (collected == needed) // Stop collecting data, save data
				{
//					char uart_buf[1000];
//					int uart_buf_len;
//				    int i;
//
//				    for (i = 0; i < 25; i++) {
//				        char num[6]; // allocate space for a 5-digit number and a null terminator
//				        sprintf(num, "%hu", ADCData[i]); // convert uint16_t to string
//				        strcat(uart_buf, num); // concatenate string with current number
//				        if (i < 24) { // add a comma after every element except the last one
//				            strcat(uart_buf, ",");
//				        }
//				        if (i > 23) { // add a comma after every element except the last one
//							strcat(uart_buf, ",");
//							sprintf(num, "%hu", slowCounter);
//							strcat(uart_buf,num);
//							strcat(uart_buf, "\n");
//						}
//				    }
				    //sprintf(uart_buf, "%d\n", slowCounter);
					//***********This block sends data over UART for testing purposes**************************************************************************************************************************
//					uart_buf_len = strlen(uart_buf);//sprintf(uart_buf, "%d", ADCData[127]);
//					HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,strlen(uart_buf),100);
//					memset(uart_buf,0,uart_buf_len);
					//uart_buf = "";
					//uart_buf_len = 0;
    				//uart_buf_len = sprintf(uart_buf,"\n");
//					HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
					//*****************************************************************************************************************************************************************************************
					collected = 0; // Resets Collected to 0 for next data collection cycle
					optReady = false; // Prevents code from trying to collect from ADC until reset to true
					//************this is where we save the data***********************************************************************************************************************************************;
				}
				else if (counter == 160) // reset after one data collection cycle
				{
					counter = 0;
					counterPrevious = 0;
					state = wait;
					sendData = true;
				}
	  		    if (counter == 20) //Sets VNA_TRIG_Pin low without interrupting timing of H-Chip collection
	  		    {
	  		    	HAL_GPIO_WritePin(GPIOB,VNA_TRIG_Pin,GPIO_PIN_RESET);
	  		    }
				break;
	  	  case wait:
	  		  	// Just waits for 50Hz timer to switch state back to collect and checks LEDs once a second
	  		  	 if (sendData)
				 {
					for (int i = 1; i < 129; i++) {
						//char num[6]; // allocate space for a 5-digit number and a null terminator
						sprintf(num, "%hu", ADCData[i]); // convert uint16_t to string
						strcat(uart_buf, num); // concatenate string with current number
						if (i < 128) { // add a comma after every element except the last one
							strcat(uart_buf, ",");
						}
						if (i == 128) { // add a comma after every element except the last one
//							strcat(uart_buf, ",");
//							sprintf(num, "%hu", slowCounter);
//							strcat(uart_buf,num);
							strcat(uart_buf, "\n");
						}
					}
					uart_buf_len = strlen(uart_buf);//sprintf(uart_buf, "%d", ADCData[127]);
					HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,strlen(uart_buf),50);
					memset(uart_buf,0,uart_buf_len);
					sendData = false;
				 }
	  		  	 if (slowCounter > 50)
	  		  	{
	  		  		slowCounter = 0;
	  		  		//state = checkLED;
	  		  		//seconds++;
	  		  	}
	  		  	break;
	  	  case checkLED:
//	  		  	vSense_Array[0] = vSense0;
//	  		    vSense_Array[1] = vSense1;
//	  		    vSense_Array[2] = vSense2;
//	  		    vSense_Array[3] = vSense3;
//	  		    vSense_Array[4] = vSense4;
//
//	  		    for (int i = 0; i < 5; i++) {
//	  		      if (vSense_Array[i] < Vat50mA) {
//	  		        if (!LED_Voltage_Shootover[i]) {
//	  		          LED_Voltages[i] = LED_Voltages[i] + calibration[0];
//	  		        }
//	  		        else {
//	  		          LED_Cal_Set[i]++;
//	  		          LED_Voltages[i] = LED_Voltages[i] + calibration[LED_Cal_Set[i]];
//	  		        }
//	  		      }
//	  		      else if (vSense_Array[i] > Vat50mA) {
//	  		        LED_Voltage_shootover[i] = 1;
//	  		        LED_Cal_Set[i]++;
//	  		        LED_Voltages[i] = LED_Voltages[i] - calibration[LED_Cal_Set[i]];
//	  		      }
//	  		    }
	  		  	break;

	  	  case test: //Never runs in normal code operation, just a handy place to test something out
	  		  //To use set state to test right above the while loop
		  HAL_TIM_Base_Stop_IT(&htim2);
		  HAL_TIM_Base_Stop_IT(&htim16);
//	  		uint32_t adc_value = 0;
//	  		uint32_t data_conversion_1 = 0;
//			uint32_t data_conversion_2 = 0;
//			uint32_t data_conversion_3 = 0;
//			value[0] = HAL_ADC_GetValue(&hadc1);
//			value[1] = HAL_ADC_GetValue(&hadc1);
//			value[2] = HAL_ADC_GetValue(&hadc1);

			//uint32_t value[3];
			//HAL_ADC_Start_DMA(&hadc1, value, 3); // start adc in DMA mode
			//HAL_ADC_Stop_DMA(&hadc1);

//	  		HAL_ADC_Start(&hadc1);
//
//	  		HAL_ADC_PollForConversion(&hadc1,10);
//
//	  		data_conversion_1 = HAL_ADC_GetValue(&hadc1);
//
//	  		HAL_ADC_Start(&hadc1);
//
//	  		HAL_ADC_PollForConversion(&hadc1,10);
//
//	  		data_conversion_2 = HAL_ADC_GetValue(&hadc1);
//
//	  		HAL_ADC_Start(&hadc1);
//
//			HAL_ADC_PollForConversion(&hadc1,10);
//
//			data_conversion_3 = HAL_ADC_GetValue(&hadc1);
//
//	  		HAL_ADC_Stop(&hadc1);
//	  		if (HAL_ADC_Start(&hadc1) == HAL_OK)
//	  		{
//	  		  if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
//	  		  {
//	  		    adc_value = HAL_ADC_GetValue(&hadc1);
//	  		  }
//	  		  HAL_ADC_Stop(&hadc1);
//	  		}
			//HAL_ADC_PollForConversion(&hadc1,10);

//	  		uart_buf_len = sprintf(uart_buf, "%d", value[0]);
//			HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
//			uart_buf_len = sprintf(uart_buf, "\n");
//			HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
//			HAL_Delay(50);
//	  		  HAL_TIM_Base_Stop_IT(&htim2);
//	  		  HAL_TIM_Base_Stop_IT(&htim16);
//
//
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  slowSPI = true;
	  		  MX_SPI2_Init();
	  		  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf1,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf2,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf3,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf4,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf5,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf6,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	  		  //hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
//
//	  		  HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
//	  		  HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
//	  		  HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
//	  		  HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
//	  		  HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
//	  		  HAL_Delay(50);

	  		  break;

	  }



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 4;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 12;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 3;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
//  if (slowSPI)
//  {
//	  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
//  }
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 200;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 50;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 200-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 10-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 2000-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 3000-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 500000;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|VNA_TRIG_Pin|LCD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPEC_RST_GPIO_Port, SPEC_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 VNA_TRIG_Pin LCD_RST_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|VNA_TRIG_Pin|LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PH15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DISP_Pin */
  GPIO_InitStruct.Pin = LCD_DISP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_DISP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS2_ID_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS2_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS2_ID_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PI8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PI13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC_CS_Pin */
  GPIO_InitStruct.Pin = ADC_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADC_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPEC_RST_Pin */
  GPIO_InitStruct.Pin = SPEC_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPEC_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_6)
	{
		//optReady = true;
	}
	else if (GPIO_Pin == GPIO_PIN_8)
	{
		//HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
		//optReady = true;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{

	if (htim == &htim2)
	{
//		HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
//		HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
//		HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
//		HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
		//HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
		//counterPrevious = counter;
		counter++;
		//HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
	}
	else if (htim == &htim16)
	{
		state = collect;
		counter = 0;
		slowCounter++;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
