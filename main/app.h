/**
 * @file app.h
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER SMART TARGET
 * @version 0.2
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define NUMBER_OF_PIEZOS 7

// Channel 1 ADC
#define PIEZO_0 ADC1_CHANNEL_2
#define PIEZO_1 ADC1_CHANNEL_3
#define PIEZO_2 ADC1_CHANNEL_4
#define PIEZO_3 ADC1_CHANNEL_5
#define PIEZO_4 ADC1_CHANNEL_7
#define PIEZO_5 ADC1_CHANNEL_8
#define PIEZO_6 ADC1_CHANNEL_9


typedef struct
{
  adc1_channel_t channel;
  int raw_read_value;
} sensor_data_t;

void init_adc_targets(void);
void init_app(void);
void run_app(void);
void app_loop(void);
void handle_hit_detect(void);
void set_adc_target_atten(uint8_t atten_idx);