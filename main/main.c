#include <stdio.h>
#include "gb_leds.h"
#include "app.c"

#define GPIO_IN 40
#define GPIO_OUT 48

void app_main(void)
{
    printf("Hello, world!\n");

    init_leds();
    led_roll_startup(100);
    vTaskDelay(30);

    gpio_set_direction(GPIO_OUT, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_OUT, 1);

    gpio_set_direction(GPIO_IN, GPIO_MODE_INPUT);

    while (1)
    {
        // uint16_t results = adc1_get_raw(ADC1_CHANNEL_4);
        // uint8_t adcVal = adc1_get_raw(ADC1_CHANNEL_2);
        uint8_t gpioValue = gpio_get_level(GPIO_IN);
        if (gpioValue > 0)
        {
            printf("GPIO Val: %d\n", gpioValue);
            gpio_set_level(GPIO_OUT, 0);
            color_flicker_target(0, IDX_CMD_RED);
            vTaskDelay(pdMS_TO_TICKS(20));
            gpio_set_level(GPIO_OUT, 1);
        }
        vTaskDelay(10);
    }
}