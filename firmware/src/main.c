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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_MESSAGE_CHARACTER_COUNT (5)

//DIGITS ARE COUNTED LEFT TO RIGHT, STARTING FROM 1
//DEFINE ACTIVE SEGMENTS FOR EACH DIGIT
#define LCD_DIGIT1_0	(0x00010307)
#define LCD_DIGIT1_1	(0x00000104)
#define LCD_DIGIT1_2	(0x08000303)
#define LCD_DIGIT1_3	(0x08000306)
#define LCD_DIGIT1_4	(0x08010104)
#define LCD_DIGIT1_5	(0x08010206)
#define LCD_DIGIT1_6	(0x08010207)
#define LCD_DIGIT1_7	(0x00000304)
#define LCD_DIGIT1_8	(0x08010307)
#define LCD_DIGIT1_9	(0x08010306)

//#define LCD_DP_1		(0x00000008)

#define LCD_DIGIT2_0	(0x06C20010)
#define LCD_DIGIT2_1	(0x02800000)
#define LCD_DIGIT2_2	(0x06400090)
#define LCD_DIGIT2_3	(0x06C00080)
#define LCD_DIGIT2_4	(0x02820080)
#define LCD_DIGIT2_5	(0x04C20080)
#define LCD_DIGIT2_6	(0x04C20090)
#define LCD_DIGIT2_7	(0x06800000)
#define LCD_DIGIT2_8	(0x06C20090)
#define LCD_DIGIT2_9	(0x06C20080)

#define LCD_DP_2		(0x00000020)

#define LCD_DIGIT3_0	(0x0000EC40)
#define LCD_DIGIT3_1	(0x00002800)
#define LCD_DIGIT3_2	(0x01006440)
#define LCD_DIGIT3_3	(0x01006C00)
#define LCD_DIGIT3_4	(0x0100A800)
#define LCD_DIGIT3_5	(0x0100CC00)
#define LCD_DIGIT3_6	(0x0100CC40)
#define LCD_DIGIT3_7	(0x00006800)
#define LCD_DIGIT3_8	(0x0100EC40)
#define LCD_DIGIT3_9	(0x0100EC00)

#define HTIM22_FREQUENCY	(32768)

#define WHEEL_CIRCUMFERENCE	(2.105)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
LCD_HandleTypeDef hlcd;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;
TIM_HandleTypeDef htim22;

/* USER CODE BEGIN PV */
volatile uint8_t magnetCounter = 0;	//count magnet passes
volatile uint32_t rotationTimerCounts = 0;	//measure time it takes for (magnetCounter) times of the wheel rotation, in timer counts
volatile uint8_t timer2sFlag = 0;	//flag to indicate 2s timer has run out
volatile uint8_t TIM22startedFlag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM21_Init(void);
static void MX_LCD_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM22_Init(void);
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
  MX_TIM21_Init();
  MX_LCD_Init();
  MX_TIM2_Init();
  MX_TIM22_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim2); //start timer

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);		//keep device ON

  uint32_t lookupDigit1[10];								//initialize array that will server as a lookup table for DIGIT1
  uint32_t lookupDigit2[10];								//initialize array that will server as a lookup table for DIGIT2
  uint32_t lookupDigit3[10];								//initialize array that will server as a lookup table for DIGIT3

  uint8_t speedDigits[UART_MESSAGE_CHARACTER_COUNT];	//initialize array containing the UART message
  uint8_t localMagnetCounter = 0;						//initialize local magnet pass counter

  float speed;

  speedDigits[0] = '0';
  speedDigits[1] = '0';
  speedDigits[2] = '.';
  speedDigits[3] = '0';
  speedDigits[4] = '\n';

  lookupDigit1[0] = LCD_DIGIT1_0;
  lookupDigit1[1] = LCD_DIGIT1_1;
  lookupDigit1[2] = LCD_DIGIT1_2;
  lookupDigit1[3] = LCD_DIGIT1_3;
  lookupDigit1[4] = LCD_DIGIT1_4;
  lookupDigit1[5] = LCD_DIGIT1_5;
  lookupDigit1[6] = LCD_DIGIT1_6;
  lookupDigit1[7] = LCD_DIGIT1_7;
  lookupDigit1[8] = LCD_DIGIT1_8;
  lookupDigit1[9] = LCD_DIGIT1_9;

  lookupDigit2[0] = LCD_DIGIT2_0;
  lookupDigit2[1] = LCD_DIGIT2_1;
  lookupDigit2[2] = LCD_DIGIT2_2;
  lookupDigit2[3] = LCD_DIGIT2_3;
  lookupDigit2[4] = LCD_DIGIT2_4;
  lookupDigit2[5] = LCD_DIGIT2_5;
  lookupDigit2[6] = LCD_DIGIT2_6;
  lookupDigit2[7] = LCD_DIGIT2_7;
  lookupDigit2[8] = LCD_DIGIT2_8;
  lookupDigit2[9] = LCD_DIGIT2_9;

  lookupDigit3[0] = LCD_DIGIT3_0;
  lookupDigit3[1] = LCD_DIGIT3_1;
  lookupDigit3[2] = LCD_DIGIT3_2;
  lookupDigit3[3] = LCD_DIGIT3_3;
  lookupDigit3[4] = LCD_DIGIT3_4;
  lookupDigit3[5] = LCD_DIGIT3_5;
  lookupDigit3[6] = LCD_DIGIT3_6;
  lookupDigit3[7] = LCD_DIGIT3_7;
  lookupDigit3[8] = LCD_DIGIT3_8;
  lookupDigit3[9] = LCD_DIGIT3_9;

  uint8_t digit1 = 0;
  uint8_t digit2 = 0;
  uint8_t digit3 = 0;

  float rotationTime = 0;	//The time it took to complete (magnetCounter) wheel rotation cycles
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  __disable_irq();	//disable interrupts while reading the two second timer flag
	  if(timer2sFlag){
		  localMagnetCounter = magnetCounter;					//copy magnet pass counter to local variable
		  magnetCounter = 0;									//reset magnet counter
		  timer2sFlag = 0;										//clear flag
		  rotationTime = (float)rotationTimerCounts/(32764);	//calculate total time it took to complete (magnetCounter) rotations
		  rotationTimerCounts=0;								//reset the rotation timer counter
		  __enable_irq();										//enable interrupts only after finishing access to the shared variables

		  //speed = localMagnetCounter*1.0525;	//calculate speed based on the number of magnet passes within the two second interval.
												//1.0525 comes from: D*pi/2s, where D is the wheel diameter, 0.67m

		  if(rotationTime == 0){
			  speed = 0;
			  //speed = (localMagnetCounter*WHEEL_CIRCUMFERENCE)/2;
		  }
		  else{
			  speed = (localMagnetCounter*WHEEL_CIRCUMFERENCE)/rotationTime;	//SPEED IS IN m/s!!!
		  }

		  //convert speed to KM/H
		  speed = speed*3.6;

		  // Extract the last two digits before the decimal
		  digit1= ((int)speed / 10) % 10;	//convert calculated digits to their ascii code by adding ascii code for '0'
		  speedDigits[0] = digit1 + '0';

		  digit2 = (int)speed % 10;
		  speedDigits[1] = digit2 + '0';

		  // Extract the first digit after the decimal point
		  digit3 = ((int)(speed * 10) % 10);
		  speedDigits[3] = digit3 + '0';

		  //HAL_UART_Transmit(&huart2, speedDigits, UART_MESSAGE_CHARACTER_COUNT, 100);

		  //write digits on LCD
		  HAL_LCD_Write(&hlcd, LCD_RAM_REGISTER0, ~0xFFFFFFFF, 0);	//clear RAM
		  HAL_LCD_Write(&hlcd, LCD_RAM_REGISTER0, ~0xFFFFFFFF, (lookupDigit1[digit1] | lookupDigit2[digit2] | lookupDigit3[digit3] | LCD_DP_2));
		  HAL_LCD_UpdateDisplayRequest(&hlcd);
	  }
	  else{
		  __enable_irq();	//enable interrupts immediately after reading flag if flag was not set
	  }

	  HAL_Delay(5);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LCD Initialization Function
  * @param None
  * @retval None
  */
static void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_17;
  hlcd.Init.Duty = LCD_DUTY_STATIC;
  hlcd.Init.Bias = LCD_BIAS_1_4;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_EXTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_0;
  hlcd.Init.DeadTime = LCD_DEADTIME_0;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_6;
  hlcd.Init.HighDrive = LCD_HIGHDRIVE_0;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

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
  htim2.Init.Prescaler = 20970-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 200;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  * @brief TIM21 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM21_Init(void)
{

  /* USER CODE BEGIN TIM21_Init 0 */

  /* USER CODE END TIM21_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM21_Init 1 */

  /* USER CODE END TIM21_Init 1 */
  htim21.Instance = TIM21;
  htim21.Init.Prescaler = 10-1;
  htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim21.Init.Period = 100;
  htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim21) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM21_Init 2 */

  /* USER CODE END TIM21_Init 2 */

}

/**
  * @brief TIM22 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM22_Init(void)
{

  /* USER CODE BEGIN TIM22_Init 0 */

  /* USER CODE END TIM22_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM22_Init 1 */

  /* USER CODE END TIM22_Init 1 */
  htim22.Instance = TIM22;
  htim22.Init.Prescaler = 64-1;
  htim22.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim22.Init.Period = 65535;
  htim22.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim22.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim22) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim22, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim22, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM22_Init 2 */

  /* USER CODE END TIM22_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if (GPIO_Pin == GPIO_PIN_5){	//check if interrupt came from pin 5 (no need to check the port as PA5 is currently the only 0th pin in any port used for interrupts)

		__HAL_TIM_SET_COUNTER(&htim21, 0);	// Reset the timer to count from 0 each time there is an interrupt on PA0
		HAL_TIM_Base_Start_IT(&htim21);  // Start the timer WITH interrupt capability (does nothing if timer is already started)
	}
	if (GPIO_Pin == GPIO_PIN_4){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);		//latch device OFF
	}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){ //if timer 2 created the interrupt
		timer2sFlag = 1;
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
	}
	if(htim->Instance == TIM21){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) {	//check state of PA0 (REED is connected to this pin)
			// The pin is in a high logic state - REED SWITCH OPEN
			HAL_TIM_Base_Stop(&htim21);	//stop the debounce timer

			//do nothing else
		}
		else {
			// The pin is in a low logic state - REED SWITCH CLOSED
			HAL_TIM_Base_Stop(&htim21);	//stop the debounce timer

			//measure time between two consecutive magnet passes:
			if(TIM22startedFlag){	//timer TIM22 already started
				rotationTimerCounts = rotationTimerCounts + __HAL_TIM_GetCounter(&htim22);
				magnetCounter++;

				HAL_TIM_Base_Stop(&htim22);
				__HAL_TIM_SET_COUNTER(&htim22, 0);	// Reset the timer
				HAL_TIM_Base_Start_IT(&htim22);  	// Start the timer WITH interrupt capability

			}
			else{
				__HAL_TIM_SET_COUNTER(&htim22, 0);	// Reset the timer
				HAL_TIM_Base_Start_IT(&htim22);  // Start the timer WITH interrupt capability

				TIM22startedFlag = 1;

			}
		}
	}
  if(htim->Instance == TIM22){
	  //if TIM22 has run out (after 2s), stop and reset it. Do not restart.
	  HAL_TIM_Base_Stop(&htim22);
	  __HAL_TIM_SET_COUNTER(&htim22, 0);	// Reset the timer

	  TIM22startedFlag = 0;
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
