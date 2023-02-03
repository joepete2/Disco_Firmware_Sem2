/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_SCLK_Pin GPIO_PIN_6
#define ADC_SCLK_GPIO_Port GPIOB
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define VNA_TRIG_Pin GPIO_PIN_7
#define VNA_TRIG_GPIO_Port GPIOB
#define SPEC_TRIG_BUFF_Pin GPIO_PIN_8
#define SPEC_TRIG_BUFF_GPIO_Port GPIOI
#define SPEC_STRT_BUFF_Pin GPIO_PIN_6
#define SPEC_STRT_BUFF_GPIO_Port GPIOE
#define ADC_MISO_Pin GPIO_PIN_3
#define ADC_MISO_GPIO_Port GPIOG
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define VSENSE2_Pin GPIO_PIN_8
#define VSENSE2_GPIO_Port GPIOF
#define VSENSE1_Pin GPIO_PIN_0
#define VSENSE1_GPIO_Port GPIOC
#define VCP_TX_Pin GPIO_PIN_10
#define VCP_TX_GPIO_Port GPIOB
#define VCP_RX_Pin GPIO_PIN_11
#define VCP_RX_GPIO_Port GPIOB
#define VSENSE5_Pin GPIO_PIN_3
#define VSENSE5_GPIO_Port GPIOC
#define ADC_CS_Pin GPIO_PIN_6
#define ADC_CS_GPIO_Port GPIOA
#define SPEC_CLK_Pin GPIO_PIN_12
#define SPEC_CLK_GPIO_Port GPIOD
#define SPEC_RESET_Pin GPIO_PIN_13
#define SPEC_RESET_GPIO_Port GPIOD
#define VSENSE3_Pin GPIO_PIN_0
#define VSENSE3_GPIO_Port GPIOA
#define VSENSE4_Pin GPIO_PIN_1
#define VSENSE4_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
