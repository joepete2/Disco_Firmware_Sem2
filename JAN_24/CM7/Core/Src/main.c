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
//#define OPT_CHANNEL_COUNT 128 //Matthew's code
//#define IMP_FREQ_COUNT 500
//#define LED_ON_CYCLES 50
//#define LED_TOTAL_CYCLES 100
//​
//typedef struct Data {
//  u_int16_t cycle_count;
//  double coreFreq;
//  bool led_on;
//  u_int16_t opt_start_noise_bit;
//  u_int16_t opt_data[OPT_CHANNEL_COUNT];
//  u_int16_t opt_end_noise_bit;
//  u_int8_t imp_real_data[IMP_FREQ_COUNT];
//  u_int8_t imp_imag_data[IMP_FREQ_COUNT];
//  char filename[64];
//} Data;
//data_default = { 0, 0.0, false, 0, {0}, 0, {0}, {0}, "data.csv" };
//​
//struct state;
//typedef void state_fn(struct state *);
//typedef struct state {
//  state_fn * next;
//  Data * data;
//} State;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//
//#ifndef HSEM_ID_0
//#define HSEM_ID_0 (0U) /* HW semaphore 0*/
//#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
bool optReady = false;
uint16_t spi_read_buf;
uint16_t spi_destroy_buf;
int counter = 0;
int collected = 0;
int needed = 130;
int counterPrevious = 0;
int fastCounter = 0;
int specResetCounter = 0;
enum states{collect,wait,test}state;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
TIM_HandleTypeDef htim2;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//state_fn read, write;  //Matthew's code
//pthread_t optThread, impThread;
//bool LED_On = false;
//int LED_Counter = 0;
//​
//void triggerVNA() {
//  return;
//  // Sends high signal to VNA
//}
//​
//void *readOpt(void * data) {
//  Data * dataPtr = (Data *) data;
//  dataPtr->led_on = LED_On;
//​
//  // Read Data from Pin and set data
//
//  switch (LED_Counter) {
//      case LED_ON_CYCLES:
//          // Turn LEDs off
//          LED_On = false;
//          break;
//      case LED_TOTAL_CYCLES:
//          // Turn LEDs on
//          LED_On = true;
//          LED_Counter = 0;
//          break;
//      default:
//          break;
//  }
//  LED_Counter += 1;
//  dataPtr->cycle_count += 1;
//​
//  pthread_exit(NULL);
//}
//​
//void *readImp(void * data) {
//  Data * dataPtr = (Data *) data;
//  triggerVNA();
//​
//  // Read Data from Pin and set data
//​
//  pthread_exit(NULL);
//}
//​
//void read(State * state) {
//  pthread_create(&optThread, NULL, readOpt, (void *)state->data);
//  pthread_create(&impThread, NULL, readImp, (void *)state->data);
//  pthread_join(optThread, NULL);
//  pthread_join(impThread, NULL);
//  state->next = write;
//}
//​
//void write(State * state) {
//  // Write state->data to SS
//  state->next = read;
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	Data data = Data data_default; //Matthew's code
//	State state = { read, &data };
	char uart_buf[50];
	char spi_buf[50];
	int uart_buf_len;
	int spi_buf_len;
	//bool toggle;

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  int32_t timeout;
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
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim16);
  //HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);


  //TIM1->CCR1 = (htim1.Init.Period *1);
  TIM4->CCR1 = 5;//(htim4.Init.Period / 2);
  TIM4->CCR2 = 5;
  TIM16->CCR1 = 5;//(htim16.Init.Period *1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  const uint16_t writeBuf1 = 0b0000100010000000;
  const uint16_t writeBuf2 = 0b0000010010000000;
  const uint16_t writeBuf3 = 0b0000110010000000;
  const uint16_t writeBuf4 = 0b0000001010000000;
  const uint16_t writeBuf5 = 0b0000101010000000;
  const uint16_t writeBuf6 = 0b0000011010000000;
  uint16_t ADCData[needed];

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf1,1,100);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf2,1,100);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf3,1,100);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf4,1,100);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf5,1,100);
  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf6,1,100);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
  //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  //hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

  //char spi_read_buf[20];

  state = collect;
  while (1)
  {
	  switch(state){
	  	  case collect:
	  		    if (counter == 200) // reset after one data collection cycle
				{
					counter = 0;
					counterPrevious = 0;
					//HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
					//HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf,1,100);
					state = wait;
				}
	  		    else if (counter == 0) // set spec reset pin high to tell h-chip to send data
				{
					HAL_GPIO_WritePin(GPIOD, SPEC_RST_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, VNA_TRIG_Pin,GPIO_PIN_SET);
				}
	  		    else if (counter == 10) // set spec reset back low
				{
					HAL_GPIO_WritePin(GPIOD, SPEC_RST_Pin,GPIO_PIN_RESET);

					optReady = true;
				}
				else if (optReady && (counter > counterPrevious) && (collected < needed) && (counter > 14)) // wait for AD_SP from the h-chip then uses spi2 to read from adc
				{
					//HAL_TIM_Base_Start_IT(&htim16);
					//HAL_TIMEx_PWMN_Start(&htim16,TIM_CHANNEL_1);
					//HAL_SPI_Receive(&hspi2, &spi_read_buf, 1, 100);
					//HAL_SPI_Receive(&hspi2, &spi_read_buf, 1, 100);
					/*for(int i = 0; i < 12; i++)
					{
						HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
					}*/
					HAL_GPIO_WritePin(GPIOA,ADC_CS_Pin,GPIO_PIN_SET);
				    HAL_SPI_Receive(&hspi2, &spi_destroy_buf, 1, 100);
				    HAL_GPIO_WritePin(GPIOA,ADC_CS_Pin,GPIO_PIN_RESET);
				    //memset(spi_read_buf, 0, sizeof(spi_read_buf));
				    HAL_SPI_Receive(&hspi2, &spi_read_buf, 1, 100);
				    //HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
				    counterPrevious = counter;
				    ADCData[collected] = spi_destroy_buf;
				    collected++;
				    //optReady = false;

				}
				else if (collected == needed)
				{
					uart_buf_len = sprintf(uart_buf, "%d", ADCData[40]);
					HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
					uart_buf_len = sprintf(uart_buf, "\n");
					HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
					collected = 0;
					optReady = false;
					//this is where we save the data;
				}
	  		    if (counter == 20)
	  		    {
	  		    	HAL_GPIO_WritePin(GPIOB,VNA_TRIG_Pin,GPIO_PIN_RESET);
	  		    }
				break;
	  	  case wait:

	  		  	break;

	  	  case test:
	  		  HAL_TIM_Base_Stop_IT(&htim2);
	  		  HAL_TIM_Base_Stop_IT(&htim16);


	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	  		  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf1,1,100);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf2,1,100);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf3,1,100);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf4,1,100);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf5,1,100);
	  		  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf6,1,100);
	  		  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	  		  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	  		  //hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	  		  HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
	  		  HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
	  		  HAL_GPIO_TogglePin(GPIOA,ADC_CS_Pin);
	  		  HAL_SPI_Receive(&hspi2, (uint16_t *)spi_read_buf, 1, 100);
	  		  HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
	  		  HAL_Delay(50);

	  		  break;

	  }


////	  state.next(&state); //Matthew's code
//
//	  uart_buf_len = sprintf(uart_buf, "S\n");
//	  spi_buf_len = sprintf(spi_buf, "HI THERE");
//
//
//	  //HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
//
//
//	  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf,1,100);
//	  HAL_UART_Transmit(&huart3,(uint8_t *)uart_buf,uart_buf_len,100);
//	  HAL_Delay(5000);
//	  HAL_SPI_Transmit(&hspi2,(uint16_t *)&writeBuf2,1,100);
//	  HAL_Delay(5000);
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
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
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
  htim16.Init.Period = 2000-1;
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
  huart3.Init.BaudRate = 115200;
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

  /*Configure GPIO pin : VSENSE2_Pin */
  GPIO_InitStruct.Pin = VSENSE2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VSENSE2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VSENSE1_Pin PC3 */
  GPIO_InitStruct.Pin = VSENSE1_Pin|GPIO_PIN_3;
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

  /*Configure GPIO pins : VSENSE3_Pin VSENSE4_Pin */
  GPIO_InitStruct.Pin = VSENSE3_Pin|VSENSE4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
		//HAL_GPIO_TogglePin(GPIOI,GPIO_PIN_13);
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
