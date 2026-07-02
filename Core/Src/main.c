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
#include "fatfs.h"
#include "string.h"
#include "sd_spi.h"
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

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define CMD0 0 // GO_IDLE_STATE
#define CMD8 8 // SEND_IF_COND (check voltage range)
#define CMD17 17 // READ_SINGLE_BLOCK
#define CMD24 24 // WRITE_SINGLE_BLOCK
#define CMD55 55 // APP_CMD (prefix for ACMD)
#define CMD58 58 // READ_OCR
#define ACMD41 41 // SD_SEND_OP_COND (ACMD)

#define R1_IDLE_STATE 0x01
#define DATA_START_TOKEN 0xFE

#define SD_INIT_TIMEOUT 1000
#define SD_CMD_TIMEOUT 100
static uint8_t SD_Type = 0;

static void sd_clock_train(void) {
SD_CS_HIGH();
for (int i = 0; i < 10; i++) {
sd_spi_xfer(0xFF);
}
}


static int sd_wait_ready(uint32_t timeout) {
uint8_t r;
do {
r = sd_spi_xfer(0xFF);
if (r == 0xFF) return 1; // ready
timeout--;
} while (timeout);

return 0; // timeout
}
static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg) {
uint8_t crc = 0x01; // default CRC (only required for CMD0 and CMD8 in SPI

uint8_t res;
// make sure card is ready
sd_wait_ready(SD_CMD_TIMEOUT);
sd_spi_xfer(0x40 | cmd);
  sd_spi_xfer((uint8_t)(arg >> 24));
  sd_spi_xfer((uint8_t)(arg >> 16));
  sd_spi_xfer((uint8_t)(arg >> 8));
  sd_spi_xfer((uint8_t)(arg));
  // CRC for specific commands
  if (cmd == CMD0)
	  crc = 0x95; // valid CRC for CMD0
  if (cmd == CMD8)
	  crc = 0x87; // valid CRC for CMD8 when arg == 0x1AA
  sd_spi_xfer(crc);
  // Response: up to 8 bytes until MSB is zero
  for (int i = 0; i < 8; i++) {
  res = sd_spi_xfer(0xFF);
  if (!(res & 0x80)) break; // valid response (MSB cleared)
  }
  return res; // R1 response
  }
static int sd_card_init(void) {
uint8_t r;
uint32_t timer = SD_INIT_TIMEOUT;
sd_clock_train(); // send idle clocks
SD_CS_LOW(); // select card for command
// CMD0: reset card
r = sd_send_cmd(CMD0, 0);
if (r != R1_IDLE_STATE) {
SD_CS_HIGH();
return 0; // failed to enter idle
}
r = sd_send_cmd(CMD8, 0x1AA);
if (r == R1_IDLE_STATE) {
// card supports CMD8 -> SDv2
// read remaining 4 bytes of R7 response
uint8_t resp[4];
for (int i = 0; i < 4; i++) resp[i] = sd_spi_xfer(0xFF);
if (resp[2] == 0x01 && resp[3] == 0xAA) {
// now send ACMD41 with HCS bit until card leaves idle
do {
sd_send_cmd(CMD55, 0);
r = sd_send_cmd(ACMD41, 0x40000000); // HCS = 1
timer--;
} while (r && timer);
if (timer == 0) { SD_CS_HIGH(); return 0; }
// Read OCR with CMD58 to see if it's SDHC/SDXC
r = sd_send_cmd(CMD58, 0);
if (r != 0x00) {
	SD_CS_HIGH();
	return 0;
}
uint8_t ocr[4];
for (int i = 0; i < 4; i++) ocr[i] = sd_spi_xfer(0xFF);
if (ocr[0] & 0x40) SD_Type = 2; else SD_Type = 1;

SD_CS_HIGH();
return 1; // success
}
} else {
// Might be SD v1 or MMC — attempt ACMD41 without HCS
do {
sd_send_cmd(CMD55, 0);
r = sd_send_cmd(ACMD41, 0);
timer--;
} while (r && timer);
if (timer == 0) { SD_CS_HIGH(); return 0; }
SD_Type = 1; // treat as SDSC
SD_CS_HIGH();
return 1;
}
SD_CS_HIGH();
return 0;
}
static int sd_read_sector(uint8_t* buff, uint32_t sector) {
uint8_t token;
if (SD_Type != 2) {
// Convert to byte address for SDSC cards
sector <<= 9;
}
}
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
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
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
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

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
