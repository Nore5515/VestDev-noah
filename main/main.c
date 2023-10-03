#include <stdio.h>
#include "gb_leds.h"
#include "app.c"

void app_main(void)
{
    printf("Hello, world!\n");

    init_leds();
    led_roll_startup(100);
    vTaskDelay(30);

    gpio_set_direction(36, GPIO_MODE_OUTPUT);
    gpio_set_level(36, 1);
    // gpio_set_direction(3, GPIO_MODE_INPUT);

    while (1)
    {
        // uint16_t results = adc1_get_raw(ADC1_CHANNEL_4);
        uint8_t adcVal = adc1_get_raw(ADC1_CHANNEL_2);
        printf("Cool Val: %d\n", adcVal);
        if (adcVal > 0)
        {
            printf("ADC Val: %d\n", adcVal);
            gpio_set_level(36, 0);
            color_flicker_target(0, IDX_CMD_RED);
            vTaskDelay(pdMS_TO_TICKS(20));
            gpio_set_level(36, 1);
        }
        vTaskDelay(10);
    }
}