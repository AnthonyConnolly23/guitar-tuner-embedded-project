/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "lcd.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef enum {
    NOTE_A,
    NOTE_A_SHARP,
    NOTE_B,
    NOTE_C,
    NOTE_C_SHARP,
    NOTE_D,
    NOTE_D_SHARP,
    NOTE_E,
    NOTE_F,
    NOTE_F_SHARP,
    NOTE_G,
    NOTE_G_SHARP,
    NOTE_UNKNOWN
} MusicNote;


typedef struct {
    MusicNote note;
    int offsetHz;
} TunerData;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim6;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART1_UART_Init(void);


/* USER CODE BEGIN PFP */
char *readStrFromBT( uint32_t timeout);
MusicNote stringToEnum(char* noteStr);
void updateTunerDisplay(TunerData data);
void drawScale();
void draw_arrow_down(uint16_t x, uint16_t y, uint16_t size, uint32_t color);

#define BUFF_SIZE 32
char rx_buff_from_BlueTooth_module[BUFF_SIZE];
TunerData currentTunerState;
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

  /* Reset of all peripherals, Initializes  Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  LCD_init();

  /* USER CODE BEGIN 2 */
  LCD_SetRotation(3);
  /* USER CODE END 2 */
  drawScale();

  /* Store the last note sent to the Nucleo */
  MusicNote tempNote = NOTE_UNKNOWN;

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

      char* btString = readStrFromBT(100);

      if (btString != NULL)
      {
          /* parse string */
          char* notePart = strtok(btString, ",");
          char* offsetPart = strtok(NULL, ",");

          if (notePart != NULL && offsetPart != NULL) {

        	  /* convert string note to enum */
              MusicNote noteEnum = stringToEnum(notePart);

              if (noteEnum != tempNote){
            	  UG_FillFrame(0, 0, 319, 180, C_BLACK);
            	  tempNote = noteEnum;
              }
              // Convert string offset to integer
              int offsetVal = atoi(offsetPart);

              TunerData data = {noteEnum, offsetVal};

              updateTunerDisplay(data);

          }

      }
  }
}

/**
 * @brief Print the scale under the note display
 * @retval MusicNote enum value
 */

void drawScale() {

    int y_pos = 215;
    int center_x = 160;
    int radius = 8;
    int spacing = 26;

    /* iterate from -5 to +5 (11 total circles) */
    for (int i = -5; i <= 5; i++) {

        int x_curr = center_x + (i * spacing);
        UG_COLOR c;
        int dist = abs(i);

        if (dist == 0) {
        	//center circle
            c = C_GREEN;
        } else if (dist <= 3) {
        	//3 yellows on each side
            c = C_YELLOW;
        } else {
        	//outer circles
            c = C_RED;
        }
        // Draw the solid color "LED"
        UG_FillCircle(x_curr, y_pos, radius, c);

    }
}

/**
 * @brief Get note from input character
 * @retval None
 */
MusicNote stringToEnum(char* noteStr) {
    if (strcmp(noteStr, "A") == 0) return NOTE_A;
    if (strcmp(noteStr, "A#") == 0) return NOTE_A_SHARP;
    if (strcmp(noteStr, "B") == 0) return NOTE_B;
    if (strcmp(noteStr, "C") == 0) return NOTE_C;
    if (strcmp(noteStr, "C#") == 0) return NOTE_C_SHARP;
    if (strcmp(noteStr, "D") == 0) return NOTE_D;
    if (strcmp(noteStr, "D#") == 0) return NOTE_D_SHARP;
    if (strcmp(noteStr, "E") == 0) return NOTE_E;
    if (strcmp(noteStr, "F") == 0) return NOTE_F;
    if (strcmp(noteStr, "F#") == 0) return NOTE_F_SHARP;
    if (strcmp(noteStr, "G") == 0) return NOTE_G;
    if (strcmp(noteStr, "G#") == 0) return NOTE_G_SHARP;
    return NOTE_UNKNOWN;
}

/**
 * @brief Draw sharp symbol
 * @retval None
 */
void drawSharp() {
    int sx = 230;
    int ex = 290;
    int top = 50;
    int bot = 160;
    int thick = 10;

    // Left vertical
    UG_FillFrame(sx, top, sx + thick, bot, C_WHITE);
    // Right vertical
    UG_FillFrame(ex - thick, top, ex, bot, C_WHITE);
    // Upper horizontal
    int y1 = 80;
    UG_FillFrame(sx - 10, y1, ex + 10, y1 + thick, C_WHITE);
    // Lower horizontal
    int y2 = 120;
    UG_FillFrame(sx - 10, y2, ex + 10, y2 + thick, C_WHITE);
}

void updateTunerDisplay(TunerData data) {

    int x_left = 110, x_right = 210;
    int y_top = 30, y_bot = 150;
    int thick = 12;
    int y_mid = y_top + (y_bot - y_top) / 2;

    int safe_offset = data.offsetHz;
    UG_COLOR fc = C_WHITE;

    if (safe_offset == 0){
    	fc = C_GREEN;
    }

    switch (data.note) {

        case NOTE_A:
        case NOTE_A_SHARP: {
            //Left vertical leg
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            //Right vertical leg
            UG_FillFrame(x_right - thick, y_top, x_right, y_bot, fc);
            //Top horizontal
            UG_FillFrame(x_left, y_top, x_right, y_top + thick, fc);
            //Crossbar
            UG_FillFrame(x_left + thick, y_mid - thick/2, x_right - thick, y_mid + thick/2, fc);

            if (data.note == NOTE_A_SHARP) drawSharp();
        } break;

        case NOTE_B: {
            //Left vertical spine
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            //Top horizontal
            UG_FillFrame(x_left, y_top, x_right - 10, y_top + thick, fc);
            //Middle horizontal
            UG_FillFrame(x_left, y_mid - thick/2, x_right - 10, y_mid + thick/2, fc);
            //Bottom horizontal
            UG_FillFrame(x_left, y_bot - thick, x_right - 10, y_bot, fc);
            //Right vertical short bar (top half)
            UG_FillFrame(x_right-10, y_top + thick, x_right + 2, y_mid - thick/2, fc);
            //Right vertical short bar (bottom half)
            UG_FillFrame(x_right - 10, y_mid + thick/2, x_right + 2, y_bot - thick, fc);
        } break;

        case NOTE_C:
        case NOTE_C_SHARP: {
            //Left vertical
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            //Top horizontal
            UG_FillFrame(x_left, y_top, x_right, y_top + thick, fc);
            //Bottom horizontal
            UG_FillFrame(x_left, y_bot - thick, x_right, y_bot, fc);

            if (data.note == NOTE_C_SHARP) drawSharp();
        } break;

        case NOTE_D:
        case NOTE_D_SHARP: {
            //Left vertical
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            //Top horizontal (slightly shorter to meet right curve)
            UG_FillFrame(x_left, y_top, x_right - thick, y_top + thick, fc);
            //Bottom horizontal
            UG_FillFrame(x_left, y_bot - thick, x_right - thick, y_bot, fc);
            //Right vertical (closing the D)
            UG_FillFrame(x_right - thick, y_top + thick, x_right, y_bot - thick, fc);

            if (data.note == NOTE_D_SHARP) drawSharp();
        } break;

        case NOTE_E: {
            // Left vertical
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            // Top horizontal
            UG_FillFrame(x_left, y_top, x_right, y_top + thick, fc);
            // Middle horizontal
            UG_FillFrame(x_left, y_mid - thick/2, x_right - 20, y_mid + thick/2, fc);
            // Bottom horizontal
            UG_FillFrame(x_left, y_bot - thick, x_right, y_bot, fc);
        } break;



        case NOTE_F:
        case NOTE_F_SHARP: {
            // Left vertical
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            // Top horizontal
            UG_FillFrame(x_left, y_top, x_right, y_top + thick, fc);
            // Middle horizontal
            UG_FillFrame(x_left, y_mid - thick/2, x_right - 20, y_mid + thick/2, fc);

            if (data.note == NOTE_F_SHARP) drawSharp();
        } break;

        case NOTE_G:
        case NOTE_G_SHARP: {
            // Left vertical
            UG_FillFrame(x_left, y_top, x_left + thick, y_bot, fc);
            // Top horizontal
            UG_FillFrame(x_left, y_top, x_right, y_top + thick, fc);
            // Bottom horizontal
            UG_FillFrame(x_left, y_bot - thick, x_right, y_bot, fc);
            // Right vertical (Lower half only)
            UG_FillFrame(x_right - thick, y_mid, x_right, y_bot, fc);
            // Inward middle dash
            UG_FillFrame(x_right - thick - 30, y_mid - thick/2, x_right - thick, y_mid + thick/2, fc);

            if (data.note == NOTE_G_SHARP) drawSharp();
        } break;

        default:
            UG_FillFrame(0, 0, 319, 165, C_BLACK);
            break;

    }

    /* Draw the offset arrow */
    static int prev_arrow_x = -1;

	/* Constants for the Scale */
	int center_x = 160;
	int spacing = 26;
	int arrow_y = 180; // Position above the circles
	int arrow_size = 20;


	if(safe_offset > 5) safe_offset = 5;
	if(safe_offset < -5) safe_offset = -5;

	int current_arrow_x = center_x + (safe_offset * spacing);


	if (prev_arrow_x != -1 && prev_arrow_x != current_arrow_x) {

		UG_FillFrame(prev_arrow_x - (arrow_size/2 + 2), arrow_y,
					 prev_arrow_x + (arrow_size/2 + 2), arrow_y + arrow_size, C_BLACK);
	}

	if (prev_arrow_x != current_arrow_x) {

		uint32_t arrow_color = (safe_offset == 0) ? C_GREEN : C_WHITE;
		draw_arrow_down(current_arrow_x, arrow_y, arrow_size, arrow_color);
		prev_arrow_x = current_arrow_x;
	}
}


void draw_arrow_down(uint16_t x, uint16_t y, uint16_t size, uint32_t color)
{
    // arrow shaft
    uint16_t shaft_width = size / 3;
    uint16_t shaft_height = size / 2;

    UG_FillFrame(x - (shaft_width/2), y, x + (shaft_width/2), y + shaft_height, color);

    // arrow head
    int head_start_y = y + shaft_height;
    int head_height = size - shaft_height;

    for (int i = 0; i < head_height; i++) {
        int width = (head_height - i) * 2; // Taper the width
        UG_DrawLine(x - width/2, head_start_y + i, x + width/2, head_start_y + i, color);
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */


/**
  * @brief  Function to read data from BlueTooth module via UART1
  * @param  timeout: timeout value in milliseconds
  * @retval pointer to the received data buffer
  */

char *readStrFromBT(uint32_t timeout)
{
  memset(rx_buff_from_BlueTooth_module, 0, BUFF_SIZE);
  int idx = 0;
  uint8_t received_char;
  uint32_t startTick = HAL_GetTick();

  while ((HAL_GetTick() - startTick) < timeout)
  {
      if (HAL_UART_Receive(&huart1, &received_char, 1, 10) == HAL_OK)
      {

          rx_buff_from_BlueTooth_module[idx] = received_char;
          idx++;

          if (received_char == '\n')
          {
              break;
          }

          if (idx >= BUFF_SIZE - 1) break;
      }
  }

  rx_buff_from_BlueTooth_module[idx] = '\0';

  if (idx > 0) {
      return rx_buff_from_BlueTooth_module;
  } else {
      return NULL;
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
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
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /*Configure GPIO pins : LD2_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
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
