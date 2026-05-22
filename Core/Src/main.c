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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "proximity11.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint8_t interrupt_reg_value;
static uint8_t als_flag = 0;
static uint8_t ps_flag = 0;
//char warning_ps[] = "Proximity detected!\r\n";
//char warning_als[] = "Light in room detected!\r\n";
char command_warning_ps[] = "AT+MQTTPUB=0,\"test_proximity11\",\"Proximity detected!\",1,0\r\n";
char command_warning_als[] = "AT+MQTTPUB=0,\"test_proximity11\",\"Light in room detected!\",1,0\r\n";
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)(&ch), 1, 10);
	return ch;
}
void application_init (void)
{
    uint8_t init_status;


    init_status = proximity11_default_cfg( &hi2c2 );

    if(init_status == 1)
	{
    	printf("Application init failed!\r\n");
	}
	else if (init_status == 0)
	{
		printf("Application init success!\r\n");
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if(GPIO_Pin == GPIO_PIN_2)
	{


		proximity11_get( &hi2c2, PROXIMITY11_INTERRUPT, &interrupt_reg_value, 1 );
		if( (interrupt_reg_value & 0x80 ) == 0x80)
		{
			//printf("Proximity detected!\r\n");
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
			ps_flag = 1;
		}
		else{
			ps_flag = 0;
		}

		if ( (interrupt_reg_value & 0x40 ) == 0x40)
		{
			//printf("Light in room detected!\r\n");
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
			als_flag = 1;
		}
		else{
			als_flag = 0;
		}

	}
}
uint8_t rx_data2[10], rx_data3[10];
uint8_t tx_data2[10], tx_data3[10];
HAL_StatusTypeDef my_status;

void ConnectToWiFi(void){
	char command1[] = "AT\r\n";
	char command2[] = "AT+CWMODE=3\r\n";
	char command3[] = "AT+CWJAP=\"Lab18a\",\"elektron1ka\"\r\n";
	char command4[] = "AT+MQTTUSERCFG=0,1,\"aleksandar1\",\"aleks\",\"aleks\",0,0,\"\"\r\n";
	char command5[] = "AT+MQTTUSERCFG=0,1,\"aleksandar1\",\"aleks\",\"aleks\",0,0,\"\"\r\n";
	char command6[] = "AT+MQTTCONN=0,\"broker.hivemq.com\",1883,1\r\n";
	char command7[] = "AT+MQTTSUB=0,\"test_proximity11\",1\r\n";

	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command1, strlen(command1), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command2, strlen(command2), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command3, strlen(command3), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command4, strlen(command4), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command5, strlen(command5), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command6, strlen(command6), HAL_MAX_DELAY);
	HAL_Delay(1500);
	my_status = HAL_UART_Transmit(&huart3, (uint8_t *)&command7, strlen(command7), HAL_MAX_DELAY);
	HAL_Delay(2000);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		tx_data3[0] = rx_data2[0];
		my_status = HAL_UART_Receive_IT(&huart2,  (uint8_t *)&rx_data2, 1);
		HAL_UART_AbortTransmit_IT(&huart3);
		my_status = HAL_UART_Transmit_IT(&huart3,  (uint8_t *)&tx_data3, 1);
	}
	else if (huart->Instance == USART3)
	{
		tx_data2[0] = rx_data3[0];
		my_status = HAL_UART_Receive_IT(&huart3,  (uint8_t *)&rx_data3, 1);
		HAL_UART_AbortTransmit_IT(&huart2);
		my_status = HAL_UART_Transmit_IT(&huart2,  (uint8_t *)&tx_data2, 1);
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
//	uint16_t ps_value;
//	float als_value;
//	char ps_str[100];
//	char als_str[100];
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
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  application_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart2,  (uint8_t *)&rx_data2, 1);
  HAL_UART_Receive_IT(&huart3,  (uint8_t *)&rx_data3, 1);
  ConnectToWiFi();
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //proximity11_get_ps_als_values( &hi2c2, &ps_value, &als_value );
	  //sprintf(ps_str, "AT+MQTTPUB=0,\"test_proximity11\",\"ps_value: %d\",1,0\r\n", ps_value);
	  //sprintf(als_str, "AT+MQTTPUB=0,\"test_proximity11\",\"als_value: %.1f\",1,0\r\n", als_value);

	  //HAL_UART_Transmit(&huart3, (uint8_t *)&ps_str, strlen(ps_str), HAL_MAX_DELAY);
	  //HAL_Delay(200);
	  //HAL_UART_Transmit(&huart3, (uint8_t *)&als_str, strlen(als_str), HAL_MAX_DELAY);

	  if(ps_flag == 1){
		  HAL_UART_Transmit(&huart3, (uint8_t *)&command_warning_ps, strlen(command_warning_ps), HAL_MAX_DELAY);
		  HAL_Delay(1000);
	  }

	  if(als_flag == 1){
		  HAL_UART_Transmit(&huart3, (uint8_t *)&command_warning_als, strlen(command_warning_als), HAL_MAX_DELAY);
		  HAL_Delay(1000);
	  }



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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
