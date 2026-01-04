/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rng.c
  * @brief   This file provides code for the configuration
  *          of the RNG instances.
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
#include "rng.h"

/* USER CODE BEGIN 0 */
#include "lwrb/lwrb.h"

lwrb_t lwrb_rng;
uint8_t rng_buffer[64];

/* USER CODE END 0 */

RNG_HandleTypeDef hrng;

/* RNG init function */
void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

void HAL_RNG_MspInit(RNG_HandleTypeDef* rngHandle)
{

  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspInit 0 */

  /* USER CODE END RNG_MspInit 0 */
    /* RNG clock enable */
    __HAL_RCC_RNG_CLK_ENABLE();

    /* RNG interrupt Init */
    HAL_NVIC_SetPriority(HASH_RNG_IRQn, 14, 0);
    HAL_NVIC_EnableIRQ(HASH_RNG_IRQn);
  /* USER CODE BEGIN RNG_MspInit 1 */

  /* USER CODE END RNG_MspInit 1 */
  }
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef* rngHandle)
{

  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspDeInit 0 */

  /* USER CODE END RNG_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RNG_CLK_DISABLE();

    /* RNG interrupt Deinit */
    HAL_NVIC_DisableIRQ(HASH_RNG_IRQn);
  /* USER CODE BEGIN RNG_MspDeInit 1 */

  /* USER CODE END RNG_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit) {
  int i;
  uint8_t* temp;
  lwrb_sz_t free = 0;

  if (hrng->Instance == RNG) {
    temp = (uint8_t*)&random32bit;

    free = lwrb_get_free(&lwrb_rng);
    if(free >= 4) {
      lwrb_write(&lwrb_rng, temp, 4);
      HAL_RNG_GenerateRandomNumber_IT(hrng);
    } else {
      for(i=0; i<free; i++) {
        lwrb_write(&lwrb_rng, &temp[i], 1);
      }
    }

  }
}

void random_init(void) {
  lwrb_init(&lwrb_rng, rng_buffer, sizeof(rng_buffer));
  HAL_RNG_GenerateRandomNumber_IT(&hrng);
}

void random_get(uint8_t* out, size_t osize) {
  lwrb_sz_t read_size;
  size_t remain_size, copy_size;
  uint32_t temp;
  uint8_t* pout;

  if(out == NULL) {
    return;
  }

  __HAL_RNG_DISABLE_IT(&hrng);

  read_size = lwrb_read(&lwrb_rng, out, osize);

  remain_size = osize-read_size;
  if(remain_size == 0) {
    __HAL_RNG_ENABLE_IT(&hrng);
    return;
  }

  pout = out + read_size;
  while (remain_size > 0) {
    HAL_RNG_GenerateRandomNumber(&hrng, &temp);
    copy_size = (remain_size < 4) ? remain_size : 4;
    memcpy(pout, (uint8_t*)&temp, remain_size);

    pout += copy_size;
    remain_size -= copy_size;
  }
  __HAL_RNG_ENABLE_IT(&hrng);
}

/* USER CODE END 1 */
