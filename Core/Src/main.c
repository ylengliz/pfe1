/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stdio.h"
#include "stm32f411e_discovery_accelerometer.h"
#include "ab_sensor_hub.h"

#define DEFAULT_uhCCR1_Val  100 /* 10kHz/100 value */
#define DEFAULT_uhCCR2_Val  200 /* 10kHz/50 value */
#define DEFAULT_uhCCR3_Val  625 /* 10kHz/16 value */
#define DEFAULT_uhCCR4_Val  400 /* 10kHz/25 value */


/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
RTC_HandleTypeDef RtcHandle;
TIM_HandleTypeDef AB_TimHandle;
TIM_HandleTypeDef AL_TimHandle;
//TMsg MsgDat;
//TMsg MsgCmd;
 volatile int Message_Length;


/* USER CODE BEGIN PV */
int16_t uartBuf[3] ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
void TIM_AL_Config(void);
static uint32_t DWT_Delay_Init(void);
void AB_Init(void);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  BSP_ACCELERO_Init();

  /* Initialize cycle counter */
  DWT_Delay_Init();

  /* Initialize timers for algorithms synchronization */
  TIM_AL_Config();

  /* AlgoBuilder initialization */
  AB_Init();


  /* To start reading in case of interrupt driven flow */
  //sensor_read_request = 1;

  /* Ensure that User Button pressing will be evaluated from this point */
  //DataLoggerActive = 0;
  //DataLoggerStatusChanged = 0;


  /* Infinite loop */
  while (1)
  {



#if 0
	  BSP_ACCELERO_GetXYZ( uartBuf);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uartBuf, 3, HAL_MAX_DELAY);
	   HAL_Delay(2000);
#endif

  }
  /* USER CODE END 3 */
}

void AB_Init(void)
{
	Sensor_Hub_Init(0, 10, 1);
	Accelero_Init();
	Message_Length = 12;
}
/**
  * @brief  TIM_AB start timer
  * @param  None
  * @retval None
  */
void TIM_AB_Start(void)
{
  HAL_TIM_Base_Start_IT(&AB_TimHandle);
}

/**
  * @brief  TIM_AB stop timer
  * @param  None
  * @retval None
  */
void TIM_AB_Stop(void)
{
  HAL_TIM_Base_Stop_IT(&AB_TimHandle);
}

/**
  * @brief  TIM_AL start timer
  * @param  None
  * @retval None
  */
void TIM_AL_Start(void)
{
  HAL_TIM_OC_Start_IT(&AL_TimHandle, TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&AL_TimHandle, TIM_CHANNEL_2);
  HAL_TIM_OC_Start_IT(&AL_TimHandle, TIM_CHANNEL_3);
  HAL_TIM_OC_Start_IT(&AL_TimHandle, TIM_CHANNEL_4);
}

/**
  * @brief  TIM_AL stop timer
  * @param  None
  * @retval None
  */
void TIM_AL_Stop(void)
{
  HAL_TIM_OC_Stop_IT(&AL_TimHandle, TIM_CHANNEL_1);
  HAL_TIM_OC_Stop_IT(&AL_TimHandle, TIM_CHANNEL_2);
  HAL_TIM_OC_Stop_IT(&AL_TimHandle, TIM_CHANNEL_3);
  HAL_TIM_OC_Stop_IT(&AL_TimHandle, TIM_CHANNEL_4);
}


/**
  * @brief  TIM_AL config function
  * @param  None
  * @retval None
  * @details This function initialize the timer used to synchronize the enabled algorithms
  */
void TIM_AL_Config(void)
{
  uint32_t uwPrescalerValue;
  TIM_OC_InitTypeDef sConfig;

  /* Compute the prescaler value to have TIM1 counter clock equal to 10 KHz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock / 10000) - 1);

  /* Set TIM1 instance */
  AL_TimHandle.Instance = TIM1;
  AL_TimHandle.Init.Prescaler = uwPrescalerValue;
  AL_TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  AL_TimHandle.Init.Period = 0xFFFF;
  AL_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_OC_Init(&AL_TimHandle);

  /* Configure the Output Compare channels */
  /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_TOGGLE;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

  /* Output Compare Toggle Mode configuration: Channel1 */
  sConfig.Pulse = DEFAULT_uhCCR1_Val;
  HAL_TIM_OC_ConfigChannel(&AL_TimHandle, &sConfig, TIM_CHANNEL_1);

  /* Output Compare Toggle Mode configuration: Channel2 */
  sConfig.Pulse = DEFAULT_uhCCR2_Val;
  HAL_TIM_OC_ConfigChannel(&AL_TimHandle, &sConfig, TIM_CHANNEL_2);

  /* Output Compare Toggle Mode configuration: Channel3 */
  sConfig.Pulse = DEFAULT_uhCCR3_Val;
  HAL_TIM_OC_ConfigChannel(&AL_TimHandle, &sConfig, TIM_CHANNEL_3);

  /* Output Compare Toggle Mode configuration: Channel4 */
  sConfig.Pulse = DEFAULT_uhCCR4_Val;
  HAL_TIM_OC_ConfigChannel(&AL_TimHandle, &sConfig, TIM_CHANNEL_4);
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief  Initializes DWT_Clock_Cycle_Count for DWT_GetTickUS function
 * @retval Error DWT counter (1: clock cycle counter not started, 0: clock cycle counter works)
 */
static uint32_t DWT_Delay_Init(void)
{
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;

  /* Enable TRC */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;

  /* Enable clock cycle counter */
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;

  /* Delay 1ms */
  HAL_Delay(1);

  /* Check if clock cycle counter has started */
  if (DWT->CYCCNT)
  {
     return 0; /* Clock cycle counter started */
  }
  else
  {
    return 1; /* Clock cycle counter not started */
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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
