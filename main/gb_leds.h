/**
 * @file gb_leds.h
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER SMART TARGET - LEDS header file
 * @version 0.2
 * @date 2022-09-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <stdint.h>
// #include <led_strip.h>
#include "obe_led.h"

#define LED_TYPE LED_STRIP_WS2812
#if CONFIG_IDF_TARGET_ESP32S3
#define LED_GPIO 1 // CHASE Board
#elif CONFIG_IDF_TARGET_ESP32
#define LED_GPIO 17 // CS Board
#endif

#define CONFIG_LED_STRIP_LEN 4
#define LEDS_PER_TARGET 1
#define TARGET_PADS 5

enum
{
  IDX_CMD_OFF,
  IDX_CMD_GREEN,
  IDX_CMD_RED,
  IDX_CMD_YELLOW,
  IDX_CMD_CYAN,
  IDX_CMD_MAGENTA,
  IDX_CMD_BLUE,
  IDX_CMD_GBRED,
  IDX_CMD_GBBLUE,
  IDX_CMD_WHITE,
  IDX_CMD_GREEN_ACTIVE,
  IDX_CMD_OFF_MISS,
  IDX_CMD_RED_ALL_TARGET,
  IDX_CMD_ORANGE,
  CMD_IDX_LED
};

void gb_led_init(void);
void send_led_cmd(uint8_t ind, uint8_t state);
void set_all_leds(uint8_t r, uint8_t g, uint8_t b);
void led_color_flash(rgb_t color, size_t start, uint8_t f_count, uint16_t delay);
void radio_active_on(void);
void radio_active_connected(void);
void radio_active_updating(void);
void led_target_color(rgb_t color, size_t start);
void led_target_idx_color(rgb_t color, uint8_t target_i);
void set_target_pattern(rgb_t *pattern);
void set_target_state_pattern(uint8_t *target_state);
void gb_led_init_task(void *pvParameters);
void gb_led_mesh_init_task(void *pvParameters);
void run_led_debug(void);
void led_flash_winner(uint8_t cmd_idx);
void led_rainbow(void);
void led_app_connected(void);
void led_roll_startup(uint16_t delay);
bool get_is_led_init(void);

void led_pulse_pattern(uint8_t *cmd_pattern);
void led_set_brightness(uint8_t brightness);

void stop_rapid_flash_task(void);
void run_rapid_flash_task(uint8_t *flash_cmd);

void color_flicker_by_index(uint8_t cmd, uint8_t idx);
void color_flicker_target(uint8_t idx, uint8_t cmd);
void color_flicker_panel(uint8_t panel_idx, uint8_t cmd);
void init_leds(void);
