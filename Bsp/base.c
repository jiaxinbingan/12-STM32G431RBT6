
#include "base.h"
#include "usart.h"

uint16_t tim6_10ms = 0, tim6_100ms = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		tim6_10ms++;
		tim6_100ms++;
	}
}

void LED(uint8_t myled)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, myled << 8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

_Bool key[4];
uint8_t key_cnt;
void KEY(void)
{
	_Bool key_all;
	key[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	key[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	key[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	key[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	key_all = key[0] & key[1] & key[2] & key[3];
	if(!key_all)
	{
		if(key_cnt < 255)
			key_cnt++;
	}
	else
		key_cnt = 0;
}


float GETADC(ADC_HandleTypeDef *pin)
{
	double adc;
	HAL_ADC_Start(pin);
	adc = HAL_ADC_GetValue(pin);
	return adc * 3.3 / 4096;
}

char rx_data[50];
uint8_t rx_p;
uint8_t rx_byte;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_data[rx_p++] = rx_byte;
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}






