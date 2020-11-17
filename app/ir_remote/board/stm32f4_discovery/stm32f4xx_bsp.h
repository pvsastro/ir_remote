#ifndef __STM32F4XX_BSP_H__
#define __STM32F4XX_BSP_H__

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

/* Definitions for the rx capture timer resources */
#define CAPTURE_TIM                           TIM5
#define CAPTURE_TIM_CLK_ENABLE()              __HAL_RCC_TIM5_CLK_ENABLE()
#define CAPTURE_TIM_CHANNEL                   TIM_CHANNEL_2
#define CAPTURE_TIM_HAL_LAYER_CHANNEL_NUM     HAL_TIM_ACTIVE_CHANNEL_2

#define CAPTURE_TIM_GPIO_PORT_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define CAPTURE_TIM_GPIO_PORT                 GPIOA
#define CAPTURE_TIM_GPIO_PIN                  GPIO_PIN_1
#define CAPTURE_TIM_GPIO_AF                   GPIO_AF2_TIM5

#define CAPTURE_TIM_IRQn                      TIM5_IRQn
#define Capture_TIM_IRQHandler                TIM5_IRQHandler

/* Definitions for the tx modulation timer resources */
#define MODULATION_TIM                        TIM3
#define MODULATION_TIM_CLK_ENABLE()           __HAL_RCC_TIM3_CLK_ENABLE()

#define MODULATION_TIM_IRQn                   TIM3_IRQn
#define Modulation_TIM_IRQHandler             TIM3_IRQHandler

/* Definitions for the tx timebase timer resources */
#define TIMEBASE_TIM                          TIM4
#define TIMEBASE_TIM_CLK_ENABLE()             __HAL_RCC_TIM4_CLK_ENABLE()

#define TIMEBASE_TIM_IRQn                     TIM4_IRQn
#define Timebase_TIM_IRQHandler               TIM4_IRQHandler

void SysTick_Handler(void);
void Capture_TIM_IRQHandler(void);
void Modulation_TIM_IRQHandler(void);
void Timebase_TIM_IRQHandler(void);

#endif /* __STM32F4XX_BSP_H__ */
