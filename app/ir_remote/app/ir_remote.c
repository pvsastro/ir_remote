
#include "ir_remote.h"
#include "stm32f4xx_bsp.h"

/* Handle for the timers */
TIM_HandleTypeDef               Capture_TimHandle;
TIM_HandleTypeDef               Modulation_TimHandle;
TIM_HandleTypeDef               Timebase_TimHandle;

/* Timer Input Capture Configuration Structure declaration */
static TIM_IC_InitTypeDef       Capture_Tim_Config;

/* Slave configuration structure */
static TIM_SlaveConfigTypeDef   Capture_Tim_Slave_Config;

/* Captured Value */
static __IO uint32_t            Capture_Tim_Log[128];
/* This counter is used to index into the capture timer log */
static uint32_t                 Capture_Tim_Cntr = 0;

enum key_tx_phase {
  STR_BIT_H,
  STR_BIT_L,
  ZER_BIT_H,
  ZER_BIT_L,
  ONE_BIT_H,
  ONE_BIT_L,
  RPT_BIT_H,
  RPT_BIT_L,
  IFS,
};

static const uint16_t key_tx_phase_period[] = {
   896, /* STR_BIT_H */
   448, /* STR_BIT_L */
    56, /* ZER_BIT_H */
    56, /* ZER_BIT_L */
    56, /* ONE_BIT_H */
   168, /* ONE_BIT_L */
   896, /* RPT_BIT_H */
   224, /* RPT_BIT_L */
  4740, /* IFS       */
};

static const enum key_tx_phase key_tx_phase_sof[] = {
  STR_BIT_H, STR_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L,
};

static const enum key_tx_phase key_tx_phase_rpt[] = {
  RPT_BIT_H, RPT_BIT_L, ZER_BIT_H, ZER_BIT_L,
};
#if 0
static const enum key_tx_phase key_tx_phase_key_0[] = {
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L,
  ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L,
};
#endif
static const enum key_tx_phase key_tx_phase_key_p3[] = {
  ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L, ZER_BIT_H, ZER_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L,
  ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L,
  ZER_BIT_H, ZER_BIT_L, ONE_BIT_H, ONE_BIT_L, ONE_BIT_H, ONE_BIT_L,
};

static void SystemClock_Config(void);
static void Error_Handler(void);

int main(void)
{
  /* STM32F4xx HAL library initialization:
   * - Configure the Flash prefetch, instruction and Data caches
   * - Configure the Systick to generate an interrupt each 1 msec
   * - Set NVIC Group Priority to 4
   * - Global MSP (MCU Support Package) initialization
   */
  if(HAL_Init()!= HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  /* Configure LED4 GPIO */
  BSP_LED_Init(LED4);

  /* Setup the input capture timer */
  Capture_TimHandle.Instance = CAPTURE_TIM;

  Capture_TimHandle.Init.Period = 65535;
  Capture_TimHandle.Init.Prescaler = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1;
  Capture_TimHandle.Init.ClockDivision = 0;
  Capture_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_IC_Init(&Capture_TimHandle) != HAL_OK)
    Error_Handler();

  Capture_Tim_Config.ICPrescaler = TIM_ICPSC_DIV1;
  Capture_Tim_Config.ICFilter = 0;
  Capture_Tim_Config.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  Capture_Tim_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  if(HAL_TIM_IC_ConfigChannel(&Capture_TimHandle,
                              &Capture_Tim_Config,
                              CAPTURE_TIM_CHANNEL) != HAL_OK)
    Error_Handler();

  Capture_Tim_Slave_Config.SlaveMode = TIM_SLAVEMODE_RESET;
  Capture_Tim_Slave_Config.InputTrigger = TIM_TS_TI2FP2;
  Capture_Tim_Slave_Config.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  if(HAL_TIM_SlaveConfigSynchronization(&Capture_TimHandle,
                                        &Capture_Tim_Slave_Config) != HAL_OK)
    Error_Handler();

  /* Setup the modulation timer */
  Modulation_TimHandle.Instance = MODULATION_TIM;

  Modulation_TimHandle.Init.Period = 5 - 1;
  Modulation_TimHandle.Init.Prescaler = (uint32_t)((SystemCoreClock / 2) / 380000) - 1;
  Modulation_TimHandle.Init.ClockDivision = 0;
  Modulation_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&Modulation_TimHandle) != HAL_OK)
    Error_Handler();

  /* Setup the timebase timer */
  Timebase_TimHandle.Instance = TIMEBASE_TIM;

  /* Set the period low to allow the interrupt to fire immediately */
  Timebase_TimHandle.Init.Period = 1;
  Timebase_TimHandle.Init.Prescaler = (uint32_t)((SystemCoreClock / 2) / 100000) - 1;
  Timebase_TimHandle.Init.ClockDivision = 0;
  Timebase_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&Timebase_TimHandle) != HAL_OK)
    Error_Handler();

  /* Start the input capture timer in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&Capture_TimHandle, CAPTURE_TIM_CHANNEL) != HAL_OK)
    Error_Handler();

  /* Start the timebase timer */
  if(HAL_TIM_Base_Start_IT(&Timebase_TimHandle) != HAL_OK)
    Error_Handler();

  /* Infinite loop */
  while (1)
  {
  }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &Capture_TimHandle && htim->Channel == CAPTURE_TIM_HAL_LAYER_CHANNEL_NUM)
  {
    /* Get the Input Capture value */
    Capture_Tim_Log[Capture_Tim_Cntr % 128] = HAL_TIM_ReadCapturedValue(htim,
                                  CAPTURE_TIM_CHANNEL) |
                                  ((Capture_Tim_Cntr + 1) << 16);
  }

  Capture_Tim_Cntr++;
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &Modulation_TimHandle)
    BSP_LED_Toggle(LED4);

  if (htim == &Timebase_TimHandle) {
    static unsigned int key_bit_cntr = 0;
    enum key_tx_phase phase = ZER_BIT_L;
    uint16_t arr;

    /* Tim3 runs continuosly, so halt the previous run */
    HAL_TIM_Base_Stop_IT(&Modulation_TimHandle);
    BSP_LED_Off(LED4);

    if (key_bit_cntr < 32) {
      phase = key_tx_phase_sof[key_bit_cntr];
    } else if (key_bit_cntr < (32 + 36)) {
      phase = key_tx_phase_key_p3[key_bit_cntr - 32];
    } else if (key_bit_cntr < (32 + 36 + 1)) {
      phase = IFS;
    } else if (key_bit_cntr < (32 + 36 + 1 + 4)) {
      phase = key_tx_phase_rpt[key_bit_cntr - 32 - 36 - 1];
    } else {
      HAL_TIM_Base_Stop_IT(&Timebase_TimHandle);
      key_bit_cntr = 0;
    }

    arr = key_tx_phase_period[phase];
    if (arr) {
      Timebase_TimHandle.Instance->ARR = arr;

      if (phase == ZER_BIT_H || phase == ONE_BIT_H ||
          phase == STR_BIT_H || phase == RPT_BIT_H)
        HAL_TIM_Base_Start_IT(&Modulation_TimHandle);

      key_bit_cntr++;
    }
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED5 on */
  BSP_LED_Init(LED5);
  BSP_LED_On(LED5);
  while(1)
  {
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
  *            PLL_M                          = 8
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

  /* The voltage scaling allows optimizing the power consumption when the
   * device is  clocked below the maximum system frequency, to update the
   * voltage scaling value regarding system frequency refer to product
   * datasheet.
   */
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
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and
   * PCLK2 clocks dividers.
   */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

#ifdef  USE_FULL_ASSERT

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
