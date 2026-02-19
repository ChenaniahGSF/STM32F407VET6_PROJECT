/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "multi_button.h"
#include "smarttimer.h"
#include "xmodem.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for NormalTask5ms */
osThreadId_t NormalTask5msHandle;
uint32_t NormalTask5msBuffer[ 256 ];
osStaticThreadDef_t NormalTask5msControlBlock;
const osThreadAttr_t NormalTask5ms_attributes = {
  .name = "NormalTask5ms",
  .cb_mem = &NormalTask5msControlBlock,
  .cb_size = sizeof(NormalTask5msControlBlock),
  .stack_mem = &NormalTask5msBuffer[0],
  .stack_size = sizeof(NormalTask5msBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for HighTask10ms */
osThreadId_t HighTask10msHandle;
uint32_t HighTask10msBuffer[ 256 ];
osStaticThreadDef_t HighTask10msControlBlock;
const osThreadAttr_t HighTask10ms_attributes = {
  .name = "HighTask10ms",
  .cb_mem = &HighTask10msControlBlock,
  .cb_size = sizeof(HighTask10msControlBlock),
  .stack_mem = &HighTask10msBuffer[0],
  .stack_size = sizeof(HighTask10msBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for LowTask50ms */
osThreadId_t LowTask50msHandle;
uint32_t LowTask50msBuffer[ 256 ];
osStaticThreadDef_t LowTask50msControlBlock;
const osThreadAttr_t LowTask50ms_attributes = {
  .name = "LowTask50ms",
  .cb_mem = &LowTask50msControlBlock,
  .cb_size = sizeof(LowTask50msControlBlock),
  .stack_mem = &LowTask50msBuffer[0],
  .stack_size = sizeof(LowTask50msBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for NormalTask20ms */
osThreadId_t NormalTask20msHandle;
uint32_t NormalTask20msBuffer[ 256 ];
osStaticThreadDef_t NormalTask20msControlBlock;
const osThreadAttr_t NormalTask20ms_attributes = {
  .name = "NormalTask20ms",
  .cb_mem = &NormalTask20msControlBlock,
  .cb_size = sizeof(NormalTask20msControlBlock),
  .stack_mem = &NormalTask20msBuffer[0],
  .stack_size = sizeof(NormalTask20msBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartNormalTask5ms(void *argument);
void StartHighTask10ms(void *argument);
void StartLowTask50ms(void *argument);
void StartNormalTask20ms(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of NormalTask5ms */
  NormalTask5msHandle = osThreadNew(StartNormalTask5ms, NULL, &NormalTask5ms_attributes);

  /* creation of HighTask10ms */
  HighTask10msHandle = osThreadNew(StartHighTask10ms, NULL, &HighTask10ms_attributes);

  /* creation of LowTask50ms */
  LowTask50msHandle = osThreadNew(StartLowTask50ms, NULL, &LowTask50ms_attributes);

  /* creation of NormalTask20ms */
  NormalTask20msHandle = osThreadNew(StartNormalTask20ms, NULL, &NormalTask20ms_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartNormalTask5ms */
/**
  * @brief  Function implementing the NormalTask5ms thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartNormalTask5ms */
void StartNormalTask5ms(void *argument)
{
  /* USER CODE BEGIN StartNormalTask5ms */
  /* Infinite loop */
  for(;;)
  {
    button_ticks();
    osDelay(5);
  }
  /* USER CODE END StartNormalTask5ms */
}

/* USER CODE BEGIN Header_StartHighTask10ms */
/**
* @brief Function implementing the HighTask10ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHighTask10ms */
void StartHighTask10ms(void *argument)
{
  /* USER CODE BEGIN StartHighTask10ms */
  /* Infinite loop */
  for(;;)
  {
    stim_mainloop();
    xmodem_receive_mainfuncion();
    osDelay(10);
  }
  /* USER CODE END StartHighTask10ms */
}

/* USER CODE BEGIN Header_StartLowTask50ms */
/**
* @brief Function implementing the LowTask50ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLowTask50ms */
void StartLowTask50ms(void *argument)
{
  /* USER CODE BEGIN StartLowTask50ms */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    osDelay(50);
  }
  /* USER CODE END StartLowTask50ms */
}

/* USER CODE BEGIN Header_StartNormalTask20ms */
/**
* @brief Function implementing the NormalTask20ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNormalTask20ms */
void StartNormalTask20ms(void *argument)
{
  /* USER CODE BEGIN StartNormalTask20ms */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    osDelay(20);
  }
  /* USER CODE END StartNormalTask20ms */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

