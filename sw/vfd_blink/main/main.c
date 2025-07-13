#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_Power   GPIO_NUM_1 //GPIO pin 1 → LED D4
#define LED_MCU     GPIO_NUM_2 //GPIO pin 2 → LED D5

static const char* TAG = "VFDClock";

// Task handles
TaskHandle_t ledBlinkTaskHandle = NULL;

// Functions
void ledBlinkTask(void *pvParameters){
    // Turn Power LED on when power is present
    gpio_set_level(LED_Power, 0); 

    while(1){
            // Toggle MCU LED (D5)
            gpio_set_level(LED_MCU, 0);
            ESP_LOGI("ledBlinkTask", "MCU LED on");
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            gpio_set_level(LED_MCU, 1);
            ESP_LOGI("ledBlinkTask", "MCU LED off");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
}

void app_main(void)
{
    ESP_LOGW(TAG, "==== START ===");

    /*  GPIO Config */
    gpio_config_t GPIO_CONF_LED;
    // Bit mask
    GPIO_CONF_LED.pin_bit_mask = (1ULL << LED_Power)|(1ULL << LED_MCU);
    // Pin mode
    GPIO_CONF_LED.mode = GPIO_MODE_OUTPUT;
    // No pull-up/pull-down resistors.
    GPIO_CONF_LED.pull_up_en = GPIO_PULLUP_DISABLE;
    GPIO_CONF_LED.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // No interrupt enabled.
    GPIO_CONF_LED.intr_type = GPIO_INTR_DISABLE;
    // Configure the GPIO with the settings.
    gpio_config(&GPIO_CONF_LED);

    /*  FreeRTOS tasks  */
    xTaskCreatePinnedToCore(
        ledBlinkTask,        // Task func
        "LED Blink task",    // Task name
        2048,                // Stack size
        NULL,                // Task params
        1,                   // Task priority
        &ledBlinkTaskHandle, // Task handle            
        1                    // Pinned to core #
    );

    // while(1){
    //     vTaskDelay(10 / portTICK_PERIOD_MS);

    // }
}
