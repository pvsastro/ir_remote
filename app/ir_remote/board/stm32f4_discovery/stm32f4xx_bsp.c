
#include "stm32f4xx_bsp.h"

extern TIM_HandleTypeDef Capture_TimHandle;
extern TIM_HandleTypeDef Modulation_TimHandle;
extern TIM_HandleTypeDef Timebase_TimHandle;

/*
 * Interrupt handlers for the CPU and the peripherals
 */

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void Capture_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Capture_TimHandle);
}

void Modulation_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Modulation_TimHandle);
}

void Timebase_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Timebase_TimHandle);
}

/*
 * ST library callbacks imlementation
 */

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Capture timer peripheral clock enable */
  CAPTURE_TIM_CLK_ENABLE();

  /* Enable GPIO clock */
  CAPTURE_TIM_GPIO_PORT_CLK_ENABLE();

  /* Configure timer channel alternate function */
  GPIO_InitStruct.Pin = CAPTURE_TIM_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = CAPTURE_TIM_GPIO_AF;
  HAL_GPIO_Init(CAPTURE_TIM_GPIO_PORT, &GPIO_InitStruct);

  /* Enable the capture timer global interrupt */
  HAL_NVIC_SetPriority(CAPTURE_TIM_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(CAPTURE_TIM_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /* Modulation timer peripheral clock enable */
  MODULATION_TIM_CLK_ENABLE();

  /* Enable the modulation timer global interrupt */
  HAL_NVIC_SetPriority(MODULATION_TIM_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(MODULATION_TIM_IRQn);

  /* Timebase timer peripheral clock enable */
  TIMEBASE_TIM_CLK_ENABLE();

  /* Enable the timebase timer global interrupt */
  HAL_NVIC_SetPriority(TIMEBASE_TIM_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(TIMEBASE_TIM_IRQn);
}
