/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "base.h"
#include "lcd.h"

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

extern uint16_t tim6_10ms, tim6_100ms;
extern _Bool key[4];
extern uint8_t key_cnt;
extern uint32_t tim2_ic2_fre;
extern float tim3_ic1_duty;
extern float tim3_ic2_duty;
extern uint32_t tim3_ic2_high, tim3_ic2_all;
extern char rx_data[50];
extern uint8_t rx_p;
extern uint8_t rx_byte;
//
uint8_t led = 0x00;
char text[10][30];
float ao1;
char rx_mes[50];
uint8_t flag_pwm = 0;
double a = 0, b = 0;
double ax, bx;
uint8_t mode = 0;
_Bool flag_x = 0;
uint16_t Pax = 20, Pbx= 20, Pf = 1000;
void RX_PRO(void)
{
	if(rx_p)
	{
		if(rx_p == 2)
		{
			if(strcmp(rx_data, "ab") == 0)
			{
				sprintf(rx_mes, "hi\n");
				HAL_UART_Transmit(&huart1, (unsigned char *)rx_mes, strlen(rx_mes), 50);
			}
		}
		else
		{
			sprintf(rx_mes, "error\n");
			HAL_UART_Transmit(&huart1, (unsigned char *)rx_mes, strlen(rx_mes), 50);
		}
		rx_p = 0;
		memset(rx_data, 0, 50);
	}
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim6);
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
		HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	LED(0x00);
  while (1)
  {
		if(tim6_10ms >= 10)
		{
			tim6_10ms = 0;
			//in
			KEY();
			//pro
			if(!key[0] && key_cnt == 2)
			{
				mode = !mode;
			}
			if(mode == 0)
			{
				if(!key[2] && key_cnt == 2)
				{
					flag_x = !flag_x;
				}
				if(!key[3] && key_cnt == 2)
				{
					flag_pwm = 1;
				}
				sprintf(text[1], "        DATA");
				sprintf(text[2], "   a:%.1f   ", a);
				sprintf(text[3], "   b:%.1f   ", b);
				sprintf(text[4], "   f:%d     ", tim2_ic2_fre);
				sprintf(text[6], "   ax:%.0f      ", ax);
				sprintf(text[7], "   bx:%.0f      ", bx);
				if(flag_x == 0)
					sprintf(text[8], "   mode:A     ");
				else if(flag_x == 1)
					sprintf(text[8], "   mode:B      ");
			}
			else if(mode == 1)
			{
				if(!key[1] && key_cnt == 2)
				{
					Pax+=10;
					Pbx+=10;
					if(Pax > 60)
						Pax = 10;
					if(Pbx > 60)
						Pbx = 10;
				}
				if(!key[2] && key_cnt == 2)
				{
					Pf+=1000;
					if(Pf > 10000)
						Pf = 1000;
				}
				sprintf(text[1], "        PAPA");
				sprintf(text[2], "   Pax:%d    ", Pax);
				sprintf(text[3], "   Pbx:%d    ", Pbx);
				sprintf(text[4], "   Pf:%d     ", Pf);
				sprintf(text[6], "                   ");
				sprintf(text[7], "                   ");
				sprintf(text[8], "                   ");
			}
			//out
			
		}
		if(tim6_100ms >= 100)
		{
			tim6_100ms = 0;
			//in
			RX_PRO();
			ao1 = GETADC(&hadc2);
			//pro
			if(flag_x == 1)
			{
				if(ao1 > 2 && flag_pwm == 0)
				{
					flag_x = 2;
					flag_pwm = 1;
				}
			}
			
			if(flag_x == 1)
			{
				if(flag_pwm == 1)
				{
					flag_pwm = 2;
					if((double)tim3_ic1_duty > 10.0 && (double)tim3_ic1_duty <= 90)
					{
						if(((double)tim3_ic1_duty - 10.0) * 2.25 > a)
							ax = ((double)tim3_ic1_duty - 10.0) * 2.25 - a;
						else
							ax = a - ((double)tim3_ic1_duty - 10.0) * 2.25;
						a = ((double)tim3_ic1_duty - 10.0) * 2.25;
					}
					else if((double)tim3_ic1_duty >= 90)
					{
						a = 180;
					}
					else if((double)tim3_ic1_duty < 10)
					{
						ax = a;
						a = 0;
					}
					
					if((double)tim3_ic2_duty > 10.0 && (double)tim3_ic2_duty <= 90)
					{
						if(((double)tim3_ic2_duty - 10.0) * 1.125 > b)
							bx = ((double)tim3_ic2_duty - 10.0) * 1.125 - b;
						else
							bx = b - ((double)tim3_ic2_duty - 10.0) * 1.125;
						b = ((double)tim3_ic2_duty - 10.0) * 1.125;
					}
					else if(tim3_ic2_duty > 90)
					{
						b = 90;
					}
					else if(tim3_ic2_duty < 10)
					{
						bx = b;
						b = 0;
					}
				}
				else if(flag_pwm == 2)
				{
					if(ao1 < 2)
						flag_pwm = 0;
				}
			}
			else
			{
				if(flag_pwm == 1)
				{
					flag_pwm = 0;
					if((double)tim3_ic1_duty > 10.0 && (double)tim3_ic1_duty <= 90)
					{
						if(((double)tim3_ic1_duty - 10.0) * 2.25 > a)
							ax = ((double)tim3_ic1_duty - 10.0) * 2.25 - a;
						else
							ax = a - ((double)tim3_ic1_duty - 10.0) * 2.25;
						a = ((double)tim3_ic1_duty - 10.0) * 2.25;
					}
					else if((double)tim3_ic1_duty > 90)
					{
						a = 180;
					}
					else if((double)tim3_ic1_duty < 10)
					{
						ax = a;
						a = 0;
					}
					
					if((double)tim3_ic2_duty > 10.0 && (double)tim3_ic2_duty <= 90)
					{
						if(((double)tim3_ic2_duty - 10.0) * 1.125 > b)
							bx = ((double)tim3_ic2_duty - 10.0) * 1.125 - b;
						else
							bx = b - ((double)tim3_ic2_duty - 10.0) * 1.125;
						b = ((double)tim3_ic2_duty - 10.0) * 1.125;
					}
					else if(tim3_ic2_duty > 90)
					{
						b = 90;
					}
					else if(tim3_ic2_duty < 10)
					{
						bx = b;
						b = 0;
					}
				}
			}
			
			if(ax > Pax)
				led |= 0x01;
			else
				led &= ~0x01;
			if(bx > Pbx)
				led |= 0x02;
			else
				led &= ~0x02;
			if(tim2_ic2_fre > Pf)
				led |= 0x04;
			else
				led &= ~0x04;
			if(!flag_x)
				led |= 0x10;
			else
				led &= ~0x10;
			
			if( (90 + b - a) < 10)
				led |= 0x20;
			else
				led &= ~0x20;
				
			//out
			LED(led);
			LCD_DisplayStringLine(Line1, (unsigned char *)text[1]);
			LCD_DisplayStringLine(Line2, (unsigned char *)text[2]);
			LCD_DisplayStringLine(Line3, (unsigned char *)text[3]);
			LCD_DisplayStringLine(Line4, (unsigned char *)text[4]);
			LCD_DisplayStringLine(Line6, (unsigned char *)text[6]);
			LCD_DisplayStringLine(Line7, (unsigned char *)text[7]);
			LCD_DisplayStringLine(Line8, (unsigned char *)text[8]);
		}
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
