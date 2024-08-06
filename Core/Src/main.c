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
#include "usb_device.h"
#include "gpio.h"
#include "adc.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_hid.h"
#include "stdbool.h"
#include "encoder.h"
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

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t midiNoteOn[4];
uint8_t midiNoteOff[4];
float _err_estimate;
uint16_t newEncoderValue = 0;
uint16_t oldEncoderValue_1 = 0;
uint16_t oldEncoderValue_3 = 0;
uint16_t oldEncoderValue_4 = 0;
NoteOnOff butON = {0x09, 0x90, 0, 0x7F};
NoteOnOff Sensing = {MIDI_ACTIVE_SENSING, 0, 0, 0};
uint16_t x = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void encoder(uint16_t newEncoderValue, uint16_t* oldEncoderValue, uint8_t num, uint32_t CR);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum {UP = 0,DOWN = 0x10}direction;
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
  MX_USB_DEVICE_Init();
	Encoder_init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		encoder(TIM1->CNT,&oldEncoderValue_1,1,TIM1->CR1);
		encoder(TIM3->CNT,&oldEncoderValue_3,3,TIM3->CR1);
		encoder(TIM4->CNT,&oldEncoderValue_4,4,TIM4->CR1);
		++x;
		if(x>65534)USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&Sensing, 4);
		
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void send_cc_message(uint8_t channel, uint8_t controller, uint8_t value) {
    uint8_t midi_data[4] = {0x0B, 0x00, 0x00, 0x00};  // MIDI CC сообщение
    midi_data[1] = 0xB0 | (channel & 0x0F);  // статусный байт
    midi_data[2] = controller & 0x7F;  // номер контроллера
    midi_data[3] = value & 0x7F;  // значение фейдера
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&midi_data, 4);// отправка MIDI сообщения
}
void send_note_message(uint8_t note){
	butON.MIDINote = note;
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&butON, 4);
}
void encoder(uint16_t newEncoderValue, uint16_t* oldEncoderValue, uint8_t num, uint32_t CR){
		if(newEncoderValue != *oldEncoderValue){
			direction =  CR & 0x10;
			if(!direction) send_cc_message(0,num,1);
			else send_cc_message(0,num,2);
			HAL_Delay(100);
			*oldEncoderValue = newEncoderValue;}
}
void delay_ms(uint16_t ms){
   RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;                                         // Включаем тактирование таймера
	 ms=ms*10;
	 TIM11->PSC = 8400-1;                                                         // устанавливаем предделитель 84,000,000 / 8399 = 10,000  0,0001s
   TIM11->ARR = ms - 1;                                                         // устанавливаем значение переполнения таймера
	 TIM11->EGR |= TIM_EGR_UG;                                                    // Генерируем Событие обновления для записи данных в регистры PSC и ARR
   TIM11->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM;                                       // Запускаем таймер записью бита CEN и устанавливаем режим Одного прохода установкой бита OPM
	 while (TIM11->CR1 & TIM_CR1_CEN);                                            // Ждем окончания счета
}// END_delay_ms

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
