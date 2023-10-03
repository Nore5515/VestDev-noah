
/**
 * @file obe_led.c
 * @author Obediah Klopfenstein (obe711@gmail.com)
 * @brief GEL BLASTER - OBE_LED
 * @version 0.2
 * @date 2023-08-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "obe_led.h"

static const char *TAG = "obe_led";

static portMUX_TYPE my_mutex = portMUX_INITIALIZER_UNLOCKED;

#define DEFAULT_LED_STRIP_PAUSE_LENGTH 200
#define GPIO_OUTPUT_PIN_SEL ((1ULL << STRIP_GPIO))
#define CHECK(x)            \
  do                        \
  {                         \
    esp_err_t __;           \
    if ((__ = x) != ESP_OK) \
      return __;            \
  } while (0)
#define CHECK_ARG(VAL)            \
  do                              \
  {                               \
    if (!(VAL))                   \
      return ESP_ERR_INVALID_ARG; \
  } while (0)

#define COLOR_SIZE(strip) (3 + ((strip)->is_rgbw != 0))

static rgb_t off;

// Write to strip
static void IRAM_ATTR write_to_strip(led_strip_t *strip, long value)
{
  long write_color = value;
  int i = 0;
  while (i < 24)
  {
    if (write_color & 0x800000)
    {
      gpio_set_level(strip->gpio, 0x01);
      gpio_set_level(strip->gpio, 0x01);
      gpio_set_level(strip->gpio, 0x0);
      gpio_set_level(strip->gpio, 0x0);
    }
    else
    {
      gpio_set_level(strip->gpio, 0x01);
      gpio_set_level(strip->gpio, 0x0);
      gpio_set_level(strip->gpio, 0x0);
      gpio_set_level(strip->gpio, 0x0);
    }
    i++;
    write_color = write_color << 1;
  }
}

// Values to code
static inline uint32_t values_to_code(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

// Write RGB to strip
void write_rgb_to_strip(led_strip_t *strip, uint8_t r, uint8_t g, uint8_t b)
{
  write_to_strip(strip, values_to_code(r, g, b));
}

// Led Init
esp_err_t led_strip_init(led_strip_t *strip)
{
  CHECK_ARG(strip && strip->length > 0);

  strip->buf = calloc(strip->length * 3, sizeof(uint8_t));
  if (!strip->buf)
  {
    ESP_LOGE(TAG, "Not enough memory");
    return ESP_ERR_NO_MEM;
  }

  gpio_config_t io_conf = {};

  // disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  // set as output mode
  io_conf.mode = GPIO_MODE_OUTPUT;
  // bit mask of the gpio pins
  io_conf.pin_bit_mask = ((1ULL << strip->gpio));
  // disable pull-down mode
  io_conf.pull_down_en = 0;
  // disable pull-up mode
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  return ESP_OK;
}

// Set Pixle
esp_err_t led_strip_set_pixel(led_strip_t *strip, size_t num, rgb_t color)
{
  CHECK_ARG(strip && strip->buf && num <= strip->length);
  size_t idx = num * COLOR_SIZE(strip);

  rgb_t scaled = strip->brightness != 0 ? rgb_scale_video(color, strip->brightness) : off;

  strip->buf[idx] = scaled.r;
  strip->buf[idx + 1] = scaled.g;
  strip->buf[idx + 2] = scaled.b;
  return ESP_OK;
}

// Flush strip
esp_err_t led_strip_flush(led_strip_t *strip)
{
  CHECK_ARG(strip && strip->buf);
  portENTER_CRITICAL(&my_mutex);
  // ets_delay_us(DEFAULT_LED_STRIP_PAUSE_LENGTH);
  for (size_t i = 0; i < strip->length; i++)
  {
    uint8_t buf_idx = i * 3;
    write_to_strip(strip, values_to_code(strip->buf[buf_idx], strip->buf[buf_idx + 1], strip->buf[buf_idx + 2]));
  }

  ets_delay_us(DEFAULT_LED_STRIP_PAUSE_LENGTH);
  portEXIT_CRITICAL(&my_mutex);
  return ESP_OK;
}

// Free strip
esp_err_t led_strip_free(led_strip_t *strip)
{
  CHECK_ARG(strip && strip->buf);
  free(strip->buf);

  return ESP_OK;
}

esp_err_t led_strip_fill(led_strip_t *strip, size_t start, size_t len, rgb_t color)
{
  CHECK_ARG(strip && strip->buf && len && start + len <= strip->length);

  for (size_t i = start; i < start + len; i++)
    CHECK(led_strip_set_pixel(strip, i, color));
  return ESP_OK;
}

void led_strip_install(void)
{
  off.r = 0;
  off.g = 0;
  off.b = 0;
}