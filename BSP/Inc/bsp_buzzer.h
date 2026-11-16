//
// Created by HYX on 2026/11/16.
//

#ifndef PROJECT1_BSP_BUZZER_H
#define PROJECT1_BSP_BUZZER_H

#include<stdint.h>

void buzzer_start();
void buzzer_flash(uint16_t volume);
void buzzer_stop();
void buzzer_freq(uint16_t ARR);
#endif //PROJECT1_BSP_BUZZER_H
