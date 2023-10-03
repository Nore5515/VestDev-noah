/**
 * @file gb_leds.c
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER SMART TARGET - LEDS
 * @version 0.2
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gb_leds.h"
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "obe_led.h"

#define LOG_LEVEL_LOCAL ESP_LOG_ERROR
static const char *TAG = "gb_leds";

static bool is_led_init = false;

static led_strip_t strip_A = {
    .type = 0,
    .length = 4,
    .gpio = 39,
    .buf = NULL,
    .brightness = 255,
};



////////////////////////////////////////////////////////////////////////////////
enum
{
  IDX_LED_0,
  IDX_LED_1,
  IDX_LED_2,
  IDX_LED_3,
  IDX_LED_4,
  TARGET_IDX_LED
};


// LED_STRIP_WS2812 (Original - PRODUCTION)
static rgb_t led_cmd_table[CMD_IDX_LED] = {
    [IDX_CMD_OFF] = {.r = 0x00, .g = 0x00, .b = 0x00},
    [IDX_CMD_GREEN] = {.r = 0x00, .g = 0xff, .b = 0x00},
    [IDX_CMD_RED] = {.r = 0xff, .g = 0x00, .b = 0x00},
    [IDX_CMD_YELLOW] = {.r = 0xff, .g = 0xff, .b = 0x00},
    [IDX_CMD_CYAN] = {.r = 0x00, .g = 0xff, .b = 0xff},
    [IDX_CMD_MAGENTA] = {.r = 0xff, .g = 0x00, .b = 0xff},
    [IDX_CMD_BLUE] = {.r = 0x00, .g = 0x00, .b = 0xff},
    [IDX_CMD_GBRED] = {.r = 0xff, .g = 0x00, .b = 0x00},
    [IDX_CMD_GBBLUE] = {.r = 0x00, .g = 0x00, .b = 0xff},
    [IDX_CMD_WHITE] = {.r = 0xff, .g = 0xff, .b = 0xff},
    [IDX_CMD_GREEN_ACTIVE] = {.r = 0x00, .g = 0xff, .b = 0x00},
    [IDX_CMD_OFF_MISS] = {.r = 0x00, .g = 0x00, .b = 0x00},
    [IDX_CMD_RED_ALL_TARGET] = {.r = 0xff, .g = 0x00, .b = 0x00},
    [IDX_CMD_ORANGE] = {.r = 0xff, .g = 0x26, .b = 0x00},
};


////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Set target color by start index.
 *
 * @param color RGB color to set
 * @param target_i start of led index
 */
void led_target_color(rgb_t color, size_t start)
{
  ESP_ERROR_CHECK(led_strip_fill(&strip_A, start, LEDS_PER_TARGET, color));
  ESP_ERROR_CHECK(led_strip_flush(&strip_A));
}

/**
 * @brief Set the color of a single target
 *
 * @param color RGB color to set
 * @param target_i Index of the target to set
 */
void led_target_idx_color(rgb_t color, uint8_t target_i)
{
  ESP_ERROR_CHECK(led_strip_fill(&strip_A, target_i * LEDS_PER_TARGET, LEDS_PER_TARGET, color));
  vTaskDelay(10 / portTICK_PERIOD_MS);
  ESP_ERROR_CHECK(led_strip_flush(&strip_A));
}

/**
 * @brief Send LED command to target
 *
 * @param target_index
 * @param cmd
 */
void send_led_cmd(uint8_t target_index, uint8_t cmd)
{
  rgb_t cmd_color = led_cmd_table[cmd];
  led_target_color(cmd_color, target_index * LEDS_PER_TARGET);
}

/**
 * @brief Set the all leds to the given color.
 *
 * @param r Red hex value
 * @param g Green hex value
 * @param b Blue hex value
 */
void set_all_leds(uint8_t r, uint8_t g, uint8_t b)
{
#if WS2811
  rgb_t color = {.r = g, .g = r, .b = b};
#else
  rgb_t color = {.r = r, .g = g, .b = b};
#endif

  ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, color));
  ESP_ERROR_CHECK(led_strip_flush(&strip_A));
}

/**
 * @brief Set the all leds to the given color.
 *
 * @param color rgb value
 */
void set_all_leds_color(rgb_t color)
{
  if (is_led_init)
  {
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, color));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
  }
}

/**
 * @brief LED color roll animation
 *
 * @param roll_colors Pointer to RGB data
 * @param color_cnt Number of RGB colors in roll_colors
 * @param delay Time in Milliseconds between each color change
 */
void led_color_roll(rgb_t *roll_colors, size_t color_cnt, uint16_t delay)
{
  for (size_t c = 0; c < color_cnt; c++)
  {
    // ESP_LOGI("led_strip", "color: %d", c);
    for (size_t i = 0; i < strip_A.length; i++)
    {
      ESP_ERROR_CHECK(led_strip_set_pixel(&strip_A, i, roll_colors[c]));
      ESP_ERROR_CHECK(led_strip_flush(&strip_A));
       //ESP_ERROR_CHECK(led_strip_set_pixel(&strip_B, i, roll_colors[c]));
      //ESP_ERROR_CHECK(led_strip_flush(&strip_B));
      vTaskDelay(pdMS_TO_TICKS(delay));
    }
  }
}

/**
 * @brief LED RGB color roll animation
 *
 * @param delay Time in Milliseconds between each color change
 */
void led_roll_rgb(uint16_t delay)
{
  rgb_t colors[] = {
      {.r = 0x00, .g = 0x00, .b = 0x00},
      {.r = 0x2f, .g = 0x00, .b = 0x00},
      {.r = 0x00, .g = 0x00, .b = 0x00},
      {.r = 0x00, .g = 0x2f, .b = 0x00},
      {.r = 0x00, .g = 0x00, .b = 0x00},
      {.r = 0x00, .g = 0x00, .b = 0x2f},
      {.r = 0x00, .g = 0x00, .b = 0x00},
      {.r = 0xff, .g = 0xff, .b = 0x00},
  };

  led_color_roll(colors, (sizeof(colors) / sizeof(rgb_t)), delay);
}

/**
 * @brief LED startup color roll animation
 *
 * @param delay Time in Milliseconds between each color change
 */
void led_roll_startup(uint16_t delay)
{
  rgb_t colors[] = {
      {.r = 0x00, .g = 0x00, .b = 0x00},
      led_cmd_table[IDX_CMD_BLUE],
      {.r = 0x00, .g = 0x00, .b = 0x00},
      led_cmd_table[IDX_CMD_MAGENTA],
      {.r = 0x00, .g = 0x00, .b = 0x00},
      led_cmd_table[IDX_CMD_CYAN],
      {.r = 0x00, .g = 0x00, .b = 0x00},
  };

  led_color_roll(colors, (sizeof(colors) / sizeof(rgb_t)), delay);
}

/**
 * @brief LED all off
 *
 */
void led_all_off(void)
{
  rgb_t all_off = {.r = 0x00,
                   .g = 0x00,
                   .b = 0x00};
  ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, all_off));
  ESP_ERROR_CHECK(led_strip_flush(&strip_A));
}

/**
 * @brief LED RGB color flasher
 *
 * @param color RGB color to flash
 * @param f_count Number of times to flash
 * @param delay Time in Milliseconds between each color change
 */
void led_color_flash(rgb_t color, size_t start, uint8_t f_count, uint16_t delay)
{
  for (uint8_t x = 0; x < f_count; x++)
  {
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, start, LEDS_PER_TARGET, color));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(delay));
    led_all_off();
    vTaskDelay(pdMS_TO_TICKS(delay));
  }
  led_target_color(color, start * LEDS_PER_TARGET);
}

/**
 * @brief Radio Active ON - Target ON not connected
 */
void radio_active_on(void)
{
  rgb_t ra_logo[TARGET_PADS] = {
      led_cmd_table[IDX_CMD_GREEN],
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_MAGENTA],
      led_cmd_table[IDX_CMD_BLUE],
  };

  // for (int i = 0; i < TARGET_PADS; i++)
  // {
  //   led_target_color(ra_logo[i], i * LEDS_PER_TARGET);
  // }
  set_target_pattern(&ra_logo[0]);
}

/**
 * @brief Radio Active CONNECTED - Target ON and connected
 */
void radio_active_connected(void)
{
  rgb_t ra_logo[TARGET_PADS] = {
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
  };

  // for (int i = 0; i < TARGET_PADS; i++)
  // {
  //   led_target_color(ra_logo[i], i * LEDS_PER_TARGET);
  // }
  set_target_pattern(&ra_logo[0]);
}

/**
 * @brief Radio Active UPDATING - Target OTA updating
 */
void radio_active_updating(void)
{
  rgb_t ra_logo[TARGET_PADS] = {
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_YELLOW],
  };

  // for (int i = 0; i < TARGET_PADS; i++)
  // {
  //   led_target_color(ra_logo[i], i * LEDS_PER_TARGET);
  // }
  set_target_pattern(&ra_logo[0]);
}


static led_strip_t * strips[7];
/**
 * @brief Initialize the LED strip
 */
void gb_led_init(void)
{
  uint8_t core = xPortGetCoreID();
  ESP_LOGI(TAG, "LED Running on core %d", core);
  led_strip_install();
  ESP_ERROR_CHECK(led_strip_init(&strip_A));
  //ESP_ERROR_CHECK(led_strip_init(&strip_B));
  strips[0] = &strip_A;
  led_set_brightness(255);
  //led_roll_startup(100);
  // radio_active_on();
}

/**
 * @brief Initialize the LED strip for mesh network
 */
void gb_led_mesh_init_task(void *pvParameters)
{
  uint8_t core = xPortGetCoreID();
  ESP_LOGI(TAG, "LED Running on core %d", core);
  led_strip_install();
  ESP_ERROR_CHECK(led_strip_init(&strip_A));
  led_set_brightness(255);
  is_led_init = true;
  vTaskDelete(xTaskGetHandle("led_init_task"));
}

void gb_led_init_task(void *pvParameters)
{
  if (is_led_init)
  {
    ESP_LOGI(TAG, "LED already initialized");
    vTaskDelete(NULL);
  }

  gb_led_init();

  is_led_init = true;
  vTaskDelete(xTaskGetHandle("led_init_task"));
}

/**
 * @brief Set target to pattern
 */
void set_target_pattern(rgb_t *pattern)
{
  if (is_led_init)
  {
    for (int i = 0; i < TARGET_PADS; i++)
    {
      // ESP_LOGI("LED_set_target_pattern", "Setting target %d to %d %d %d", i, pattern[i].r, pattern[i].g, pattern[i].b);
      ESP_ERROR_CHECK(led_strip_set_pixel(&strip_A, (i * LEDS_PER_TARGET), pattern[i]));
      led_strip_flush(&strip_A);
    }
    //
    // Might need to increase this delay if the LED's are not updating
    //
    vTaskDelay(pdMS_TO_TICKS(10));
    // ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    // led_strip_flush(&strip_A);
  }
}

/**
 * @brief Set target state pattern
 *
 * @param *target_state
 */
void set_target_state_pattern(uint8_t *target_state)
{
  rgb_t pattern[TARGET_PADS] = {
      led_cmd_table[target_state[0]],
      led_cmd_table[target_state[1]],
      led_cmd_table[target_state[2]],
      led_cmd_table[target_state[3]],
      led_cmd_table[target_state[4]],
  };
  set_target_pattern(&pattern[0]);
}

/**
 * @brief LED DEBUG
 *
 */
void run_led_debug(void)
{
  uint8_t core = xPortGetCoreID();
  ESP_LOGI(TAG, "LED DEBUG Running on core %d", core);
  led_strip_install();
  ESP_ERROR_CHECK(led_strip_init(&strip_A));
  // led_roll_startup(100);

  while (1)
  {
    // white
    set_all_leds_color(led_cmd_table[IDX_CMD_WHITE]);
    vTaskDelay(pdMS_TO_TICKS(15000));

    // blue
    set_all_leds_color(led_cmd_table[IDX_CMD_BLUE]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // green
    set_all_leds_color(led_cmd_table[IDX_CMD_GREEN]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // red
    set_all_leds_color(led_cmd_table[IDX_CMD_RED]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // magenta
    set_all_leds_color(led_cmd_table[IDX_CMD_MAGENTA]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // yellow
    set_all_leds_color(led_cmd_table[IDX_CMD_YELLOW]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // cyan
    set_all_leds_color(led_cmd_table[IDX_CMD_CYAN]);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // flicker cyan
    for (uint8_t x = 0; x < 5; x++)
    {
      set_all_leds_color(led_cmd_table[IDX_CMD_CYAN]);
      vTaskDelay(pdMS_TO_TICKS(50));
      set_all_leds_color(led_cmd_table[IDX_CMD_OFF]);
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

/**
 * @brief LED Flash winner
 *
 */
void led_flash_winner(uint8_t cmd_idx)
{
  for (uint8_t x = 0; x < 4; x++)
  {
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, led_cmd_table[cmd_idx]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, led_cmd_table[IDX_CMD_OFF]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, led_cmd_table[cmd_idx]));
  ESP_ERROR_CHECK(led_strip_flush(&strip_A));
}

bool get_is_led_init(void)
{
  return is_led_init;
}

/**
 * @brief Rainbow - Target ON not connected - NO-APP-STATE (DS_NO_APP)
 */
void led_rainbow(void)
{
  rgb_t ra_logo[TARGET_PADS] = {
      led_cmd_table[IDX_CMD_GREEN],
      led_cmd_table[IDX_CMD_YELLOW],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_MAGENTA],
      led_cmd_table[IDX_CMD_BLUE],
  };

  set_target_pattern(&ra_logo[0]);
}

/**
 * @brief App CONNECTED - Target ON and connected - APP-CONNECTED (DS_APP_CONNECTED)
 */
void led_app_connected(void)
{
  rgb_t ra_logo[TARGET_PADS] = {
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
      led_cmd_table[IDX_CMD_CYAN],
  };

  set_target_pattern(&ra_logo[0]);
}

void led_set_brightness(uint8_t brightness)
{
  memcpy(&strip_A.brightness, &brightness, sizeof(brightness));
}

void led_pulse_pattern(uint8_t *cmd_pattern)
{
  if (is_led_init)
  {
    rgb_t target_pattern[TARGET_PADS] = {
        led_cmd_table[cmd_pattern[0]],
        led_cmd_table[cmd_pattern[1]],
        led_cmd_table[cmd_pattern[2]],
        led_cmd_table[cmd_pattern[3]],
        led_cmd_table[cmd_pattern[4]],
    };

    for (uint8_t x = 0; x < 200; x += 4)
    {
      led_set_brightness(x);
      set_target_pattern(&target_pattern[0]);
      vTaskDelay(pdMS_TO_TICKS(20));
    }
    for (uint8_t x = 200; x > 0; x -= 4)
    {
      led_set_brightness(x);
      set_target_pattern(&target_pattern[0]);
      vTaskDelay(pdMS_TO_TICKS(30));
    }
  }
}

static void rapid_task(void *pvParameter)
{
  uint8_t flash_cmd = *(uint8_t *)pvParameter;

  // ESP_LOGI("LED_RAPID_TASK", "flash_cmd: %d", flash_cmd);

  if (is_led_init)
  {
    while (1)
    {

      ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_A.length, led_cmd_table[flash_cmd]));
      ESP_ERROR_CHECK(led_strip_flush(&strip_A));
      vTaskDelay(pdMS_TO_TICKS(50));
      //ESP_ERROR_CHECK(led_strip_fill(&strip_A, 0, strip_B.length, led_cmd_table[IDX_CMD_OFF]));
     // ESP_ERROR_CHECK(led_strip_flush(&strip_B));
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }

  vTaskDelete(NULL);
}

void run_rapid_flash_task(uint8_t *flash_cmd)
{
  stop_rapid_flash_task();
  xTaskCreate(rapid_task, "rapid_task", 4095, flash_cmd, 5, NULL);
}

void stop_rapid_flash_task(void)
{
  TaskHandle_t xHandle = xTaskGetHandle("rapid_task");
  if (xHandle != NULL)
  {
    vTaskDelete(xHandle);
    xHandle = NULL;
  }
}

void color_flicker_by_index(uint8_t cmd, uint8_t idx)
{
  if (is_led_init)
  {
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, idx, 1, led_cmd_table[cmd]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, idx, 1, led_cmd_table[IDX_CMD_OFF]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, idx, 1, led_cmd_table[cmd]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(&strip_A, idx, 1, led_cmd_table[IDX_CMD_OFF]));
    ESP_ERROR_CHECK(led_strip_flush(&strip_A));
    vTaskDelay(pdMS_TO_TICKS(35));
  }
}

void color_flicker_panel(uint8_t panel_idx, uint8_t cmd)
{
  if (!is_led_init)
    return;

  led_strip_set_pixel(&strip_A, panel_idx, led_cmd_table[cmd]);
  led_strip_flush(&strip_A);
  vTaskDelay(pdMS_TO_TICKS(35));
  led_strip_set_pixel(&strip_A, panel_idx, led_cmd_table[IDX_CMD_OFF]);
  led_strip_flush(&strip_A);
  vTaskDelay(pdMS_TO_TICKS(35));
  led_strip_set_pixel(&strip_A, panel_idx, led_cmd_table[cmd]);
  led_strip_flush(&strip_A);
  vTaskDelay(pdMS_TO_TICKS(35));
  led_strip_set_pixel(&strip_A, panel_idx, led_cmd_table[IDX_CMD_OFF]);
  led_strip_flush(&strip_A);
  vTaskDelay(pdMS_TO_TICKS(35));
}



void color_flicker_target_panel(led_strip_t *strip, uint8_t cmd) {
   if (is_led_init)
  {
    ESP_ERROR_CHECK(led_strip_fill(strip, 0, strip->length, led_cmd_table[cmd]));
    ESP_ERROR_CHECK(led_strip_flush(strip));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(strip, 0, strip->length, led_cmd_table[IDX_CMD_OFF]));
    ESP_ERROR_CHECK(led_strip_flush(strip));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(strip, 0, strip->length, led_cmd_table[cmd]));
    ESP_ERROR_CHECK(led_strip_flush(strip));
    vTaskDelay(pdMS_TO_TICKS(35));
    ESP_ERROR_CHECK(led_strip_fill(strip, 0, strip->length, led_cmd_table[IDX_CMD_OFF]));
    ESP_ERROR_CHECK(led_strip_flush(strip));
    vTaskDelay(pdMS_TO_TICKS(35));
  }
}

void color_flicker_target(uint8_t idx, uint8_t cmd)
{
  if (is_led_init)
  {
    
    // ADD Array of strips and call based on index
    // color_flicker_target_panel(&strip_A, cmd);
    color_flicker_target_panel(strips[idx], cmd);

  }
}

/**
 * @brief Initialize the LED strip
 *
 */
void init_leds(void)
{
  esp_log_level_set(TAG, LOG_LEVEL_LOCAL);
  xTaskCreatePinnedToCore(&gb_led_init_task, "led_init", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 1);
  vTaskDelay(pdMS_TO_TICKS(3500));
}