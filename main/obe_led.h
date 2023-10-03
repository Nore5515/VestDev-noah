
/**
 * @file obe_led.h
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER - OBE_LED
 * @version 0.2
 * @date 2023-08-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rgb.h"
#include <driver/rmt.h>

/**
 * LED strip descriptor
 */
// typedef struct
// {
//   bool is_rgbw;       ///< true for RGBW strips
//   uint8_t brightness; ///< Brightness 0..255, call ::led_strip_flush() after change.
//   size_t length;      ///< Number of LEDs in strip
//   gpio_num_t gpio;    ///< Data GPIO pin
//   uint8_t *buf;
// } led_strip_t;

/**
 * LED type
 */
typedef enum
{
  LED_STRIP_WS2812 = 0,
  LED_STRIP_SK6812,
  LED_STRIP_APA106,
  LED_STRIP_SM16703,

  LED_STRIP_TYPE_MAX
} led_strip_type_t;

typedef struct
{
  led_strip_type_t type; ///< LED type
  bool is_rgbw;          ///< true for RGBW strips
  uint8_t brightness;    ///< Brightness 0..255, call ::led_strip_flush() after change.
  size_t length;         ///< Number of LEDs in strip
  gpio_num_t gpio;       ///< Data GPIO pin
  rmt_channel_t channel; ///< RMT channel
  uint8_t *buf;
} led_strip_t;

esp_err_t led_strip_init(led_strip_t *strip);
esp_err_t led_strip_set_pixel(led_strip_t *strip, size_t num, rgb_t color);
esp_err_t led_strip_flush(led_strip_t *strip);
esp_err_t led_strip_free(led_strip_t *strip);

/**
 * @brief Set multiple LEDs to the one color
 *
 * This function does not actually change colors of the LEDs.
 * Call ::led_strip_flush() to send buffer to the LEDs.
 *
 * @param strip Descriptor of LED strip
 * @param start First LED index, 0-based
 * @param len Number of LEDs
 * @param color RGB color
 * @return `ESP_OK` on success
 */
esp_err_t led_strip_fill(led_strip_t *strip, size_t start, size_t len, rgb_t color);

/**
 * @brief Setup library (DOES NOTHING)
 *
 * This method must be called before any other led_strip methods
 */
void led_strip_install();