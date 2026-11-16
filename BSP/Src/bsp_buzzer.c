#include "bsp_buzzer.h"
#include "main.h"
#include"tim.h"

void buzzer_start()
{
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);

}

void buzzer_flash(uint16_t volume)
{
    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, volume);
}
void buzzer_stop()
{
    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);
}
void buzzer_freq(uint16_t ARR)
{
  __HAL_TIM_SET_AUTORELOAD(&htim12, ARR);

}