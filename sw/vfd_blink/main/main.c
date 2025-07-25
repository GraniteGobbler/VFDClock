/* Includes */
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "vfd_driver.h"

/* Defines */
#define LED_Power   GPIO_NUM_1 // GPIO pin 1 → LED D4
#define LED_MCU     GPIO_NUM_2 // GPIO pin 2 → LED D5
#define VFD_REFRESH_PERIOD  11111 // 8333*2 = 16666 us = 60 fps

static const char* TAG = "VFDClock";
bool mux_select = 0;
uint8_t vfd_display_number = 0;


// Handles
TaskHandle_t ledBlinkTaskHandle = NULL;
TaskHandle_t CounterTaskHandle = NULL;
QueueHandle_t VFD_DisplayQueueHandle;



// Functions
/* LED Config */
void ledConfig(void){
    /*  GPIO Config */
    gpio_config_t GPIO_CONF_LED;
    // Bit mask
    GPIO_CONF_LED.pin_bit_mask = (1ULL << LED_Power) | (1ULL << LED_MCU);
    // Pin mode
    GPIO_CONF_LED.mode = GPIO_MODE_OUTPUT;
    // No pull-up/pull-down resistors.
    GPIO_CONF_LED.pull_up_en = GPIO_PULLUP_DISABLE;
    GPIO_CONF_LED.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // No interrupt enabled.
    GPIO_CONF_LED.intr_type = GPIO_INTR_DISABLE;
    // Configure the GPIO with the settings.
    gpio_config(&GPIO_CONF_LED);
}

/* Timer callbacks */
void mux_callback(void *param){
    
    vfd_value(vfd_display_number, mux_select);
    mux_select = !mux_select;
}




/* LED Task*/
void ledBlinkTask(void *pvParameters){
    // Turn Power LED on when "power is present"
    gpio_set_level(LED_Power, 0); 

    ESP_LOGW("ledBlinkTask", "CORE ID: %d", xPortGetCoreID());

    while(1){
            // Toggle MCU LED (D5)
            gpio_set_level(LED_MCU, 0);
            // ESP_LOGI("ledBlinkTask", "MCU LED on");
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            gpio_set_level(LED_MCU, 1);
            // ESP_LOGI("ledBlinkTask", "MCU LED off");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
}

/*Counter Task*/
void CounterTask(void *pvParameters){
    uint8_t cnt = 0;
    while (1){
        
        vfd_display_number = (cnt++) % 100;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void){
    ESP_LOGW(TAG, "==== START ===");

    /* Timer config */
    const esp_timer_create_args_t mux_timer_args =
    {
        .callback = &mux_callback,
        .name = "Mux Timer Interrupt"
    };

    esp_timer_handle_t mux_timer_handle;
    esp_timer_create(&mux_timer_args, &mux_timer_handle);
    esp_timer_start_periodic(mux_timer_handle, VFD_REFRESH_PERIOD); // 8 ms period
    


    /* Init */
    ledConfig();
    vfd_init();

    
    /*  FreeRTOS tasks  */
    xTaskCreate(
        ledBlinkTask,        // Task func
        "LED Blink task",    // Task name
        2048,                // Stack size
        NULL,                // Task params
        1,                   // Task priority
        &ledBlinkTaskHandle  // Task handle            
    );

    xTaskCreate(
        CounterTask,        // Task func
        "Counter task",    // Task name
        2048,                // Stack size
        NULL,                // Task params
        1,                   // Task priority
        &CounterTaskHandle  // Task handle            
    );

    

}
