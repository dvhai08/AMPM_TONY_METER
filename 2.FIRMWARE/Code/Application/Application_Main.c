/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hld_delayus.h"
#include "hld_rcc.h"
#include "process_tonymeter.h"

#include "checksum.h"
#include "tony_meter.h"
#include <tonymeter_sparser.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define APPLICATION_ADDRESS     (uint32_t)0x8000000  //0x800E800
__IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
void Application_ISR_Relocate(void);

void Application_Init(void);
void Application_RunMain(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool gApplicationInitDone = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
const GPIO_TypeDef* MCU_GPIO_OUTPUT_PORT[MCU_MAX_GPIO_OUTPUT]=
{
		MCU_OUTPUT1_GPIO_Port, MCU_OUTPUT2_GPIO_Port, MCU_OUTPUT3_GPIO_Port,
		MCU_OUTPUT4_GPIO_Port, MCU_OUTPUT5_GPIO_Port, MCU_OUTPUT6_GPIO_Port,
		MCU_OUTPUT7_GPIO_Port, MCU_OUTPUT8_GPIO_Port, MCU_OUTPUT9_GPIO_Port,
		MCU_OUTPUT10_GPIO_Port, MCU_OUTPUT11_GPIO_Port, MCU_OUTPUT12_GPIO_Port
};

const uint16_t MCU_GPIO_OUTPUT_PIN[MCU_MAX_GPIO_OUTPUT]=
{
		MCU_OUTPUT1_Pin, MCU_OUTPUT2_Pin, MCU_OUTPUT3_Pin,
		MCU_OUTPUT4_Pin, MCU_OUTPUT5_Pin, MCU_OUTPUT6_Pin,
		MCU_OUTPUT7_Pin, MCU_OUTPUT8_Pin, MCU_OUTPUT9_Pin,
		MCU_OUTPUT10_Pin, MCU_OUTPUT11_Pin, MCU_OUTPUT12_Pin,
};

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int Application_Main(void)
{
	/* USER CODE BEGIN 1 */
	Application_ISR_Relocate();
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
	MX_DMA_Init();
	MX_CRC_Init();

	MX_USART2_UART_Init();

	//MX_IWDG_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	Application_Init();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	PRINTF("\r\nStart Superloop running ...");
	while (1)
	{
		Application_RunMain();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */
void Application_Init(void)
{
	printf_init();
	PRINTF("\r\n<------------------------------------------------------->");
	PRINTF("\r\n<---------- Tony Meter Application ---------->");
	PRINTF("\r\n<------------------------------------------------------->");
	HLD_RCC_CheckResetCause();
	printf("\r\nCore Clock Speed %d Hz",HAL_RCC_GetSysClockFreq());
//	console_serial_init();
//	console_serial_add_cmd("help", help_callback);
//	console_serial_add_cmd("setrelay", setRelay_callback);
//	console_serial_add_cmd("resete", resetEnergy_callback);
	printf_process();

	process_tonymeter_init();

	printf_process();

	gApplicationInitDone = true;
}


void Application_RunMain(void)
{
	printf_process();
	TonyMeter_Cmd_Task(&TonyMeter_CmdPhase, &TonyMeter_CmdSend, TonyMeter_CmdList, &TonyMeter_TaskTimeout);

	static uint32_t u32Count = 0;
	u32Count++;
	if(u32Count > 1000000) {
		u32Count = 0;
		printf("\r\n------------");
		TonyMeter_SendCommandNow(&TonyMeter_ReadACProcess[0]);
		TonyMeter_SendCommandNow(&TonyMeter_ReadACProcess[1]);
		TonyMeter_SendCommandNow(&TonyMeter_ReadACProcess[2]);
		TonyMeter_SendCommandNow(&TonyMeter_ReadACProcess[3]);
		printf("\r\n------------");
	}
	//HAL_IWDG_Refresh(&WATCHDOG_HANDLE);
}

void Application_ISR_Relocate(void)
{
	/* Relocate by software the vector table to the internal SRAM at 0x20000000 ***/

	/* Copy the vector table from the Flash (mapped at the base of the application
	     load address 0x08004000) to the base address of the SRAM at 0x20000000. */
	for(uint32_t i = 0; i < 48; i++)
	{
		VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
	}

	/* Enable the SYSCFG peripheral clock*/
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	/* Remap SRAM at 0x00000000 */
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
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
	PRINTF("\r\n[Error] %s line %d",file, line);
	printf_process();
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
