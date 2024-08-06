#include "encoder.h"
#include "stm32f4xx_hal.h"

void TIM3_Encoder_init(void){
	//разрешаем тактирование таймера TIM3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

       //конфигурируем выводы к которым подключается энкодер как входы с подтяжкой к питанию
	GPIOA->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
	GPIOA->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7); 
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL7_1);	
	
         //настраиваем фильтр
  TIM3->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;
        //настраиваем мультиплексор
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
        //сигнал TIxFP1 появиться по возрастающему фронту
	TIM3->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
        //счет по обоим фронтам 
	//TIM3->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
			  //счет по одному фронту 
	TIM3->SMCR |= TIM_SMCR_SMS_0;
         //максимальное значение счетчика 
	TIM3->ARR = 1000;
        //включаем счетчик
	TIM3->CR1 |= TIM_CR1_CEN ;
	//обнуляем счетный регистр
	TIM3->CNT = 0;
}
void TIM1_Encoder_init(void) {
  // разрешаем тактирование таймера TIM1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  // конфигурируем выводы к которым подключается энкодер как входы с подтяжкой к питанию
  GPIOA->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;    // Alternate function mode
	GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL8_Pos) | (1 << GPIO_AFRH_AFSEL9_Pos);    // AF1 для TIM1

        //настраиваем фильтр
  TIM1->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;
        //настраиваем мультиплексор
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
        //сигнал TIxFP1 появиться по возрастающему фронту
	TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
        //счет по обоим фронтам 
	//TIM3->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
			  //счет по одному фронту 
	TIM1->SMCR |= TIM_SMCR_SMS_0;
         //максимальное значение счетчика 
	TIM1->ARR = 1000;
        //включаем счетчик
	TIM1->CR1 |= TIM_CR1_CEN ;
	//обнуляем счетный регистр
	TIM1->CNT = 0;
}
void TIM4_Encoder_init(void){
	//разрешаем тактирование таймера TIM3
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

       //конфигурируем выводы к которым подключается энкодер как входы с подтяжкой к питанию
	GPIOB->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
	GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7); 
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL7_1);	
	
         //настраиваем фильтр
  TIM4->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;
        //настраиваем мультиплексор
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
        //сигнал TIxFP1 появиться по возрастающему фронту
	TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
        //счет по обоим фронтам 
	//TIM3->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
			  //счет по одному фронту 
	TIM4->SMCR |= TIM_SMCR_SMS_0;
         //максимальное значение счетчика 
	TIM4->ARR = 1000;
        //включаем счетчик
	TIM4->CR1 |= TIM_CR1_CEN ;
	//обнуляем счетный регистр
	TIM4->CNT = 0;
}
void Encoder_init(void){
	TIM1_Encoder_init();
	TIM3_Encoder_init();
	TIM4_Encoder_init();
}
