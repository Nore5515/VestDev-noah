/**
 * @file app.c
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER SMART TARGET
 * @version 0.2
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "app.h"
#include "gb_leds.h"

#define THRESHOLD 1000
#define SAMPLE_SIZE 80000

#define S3_ADC_ENABLED 1

static sensor_data_t sensor_target[NUMBER_OF_PIEZOS] = {
    {.channel = PIEZO_0, .raw_read_value = 0},
    {.channel = PIEZO_1, .raw_read_value = 0},
    {.channel = PIEZO_2, .raw_read_value = 0},
    {.channel = PIEZO_3, .raw_read_value = 0},
    {.channel = PIEZO_4, .raw_read_value = 0},
    {.channel = PIEZO_5, .raw_read_value = 0},
    {.channel = PIEZO_6, .raw_read_value = 0},

};

static bool isTriggered = false;
static uint8_t triggerTarget = 99;
static uint32_t sample_index = 0;

adc_atten_t atten_val[ADC_ATTEN_MAX] = {ADC_ATTEN_DB_0, ADC_ATTEN_DB_2_5, ADC_ATTEN_DB_6, ADC_ATTEN_DB_11};

void app_loop(void)
{

  while (1)
  {

    for (int i = 0; i < NUMBER_OF_PIEZOS; i++)
    {
      sensor_target[i].raw_read_value = 0;

#if S3_ADC_ENABLED
      if (!isTriggered)
      {
        sensor_target[i].raw_read_value = adc1_get_raw(sensor_target[i].channel);
      }

#endif // S3_ADC_ENABLED

      // if (sensor_target[i].raw_read_value >= TRIGGER_THRESHOLD)
      if (sensor_target[i].raw_read_value >= THRESHOLD)
      {
        isTriggered = true;
        triggerTarget = i;
        break;
      }
    }

    // if (isTriggered && sample_index < SAMPLE_SIZE)
    if (isTriggered && sample_index < SAMPLE_SIZE)
    {
      sample_index++;
    }
    // else if (isTriggered && sample_index >= SAMPLE_SIZE)
    else if (isTriggered && sample_index >= SAMPLE_SIZE)
    {
      handle_hit_detect();
      isTriggered = false;
      sample_index = 0;
      triggerTarget = 99;
    }
  }
}

void handle_hit_detect(void)
{
  if (triggerTarget < NUMBER_OF_PIEZOS)
  {

    ESP_LOGI("HIT", "Index: %d", triggerTarget);
    /**
     * DO SOMETHING HERE WHEN HIT
     *
     */
    color_flicker_target(triggerTarget, IDX_CMD_RED);
  }
}

void init_app(void)
{
  init_adc_targets();
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void run_app(void)
{

  app_loop();
}

void init_adc_targets(void)
{
  uint8_t atten_idx = 0;
#if S3_ADC_ENABLED
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_0, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_1, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_2, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_3, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_4, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_5, atten_val[atten_idx]));
  ESP_ERROR_CHECK(adc1_config_channel_atten(PIEZO_6, atten_val[atten_idx]));
#endif // S3_ADC_ENABLED
}
