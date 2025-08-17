/*     
    This is a driver for IV-22 VFD tubes, based on the SN74HC595 8-bit shift register.
    

    ---------------------   --------------------- 
    |                   |   |                   |
    |   |-----a-----|   |   |   |-----a-----|   |
    |   |           |   |   |   |           |   |
    |   b           c   |   |   b           c   |
    |   |           |   |   |   |           |   |
    |   |-----d-----|   |   |   |-----d-----|   |
    |   |           |   |   |   |           |   |
    |   e           f   |   |   e           f   |
    |   |           |   |   |   |           |   |
    |   |-----g-----|   |   |   |-----g-----|   |
    |                 h |   |                 h |
    |                   |   |                   |
    ---------------------   ---------------------

*/

#include "vfd_driver.h"

void vfd_info(void)
{
    ESP_LOGI("vfd_info", "IV-22 tube driver based on the SN74HC595 8-bit Shift Register");
}


void vfd_init(void){
    // GPIO config
    gpio_config_t GPIO_CONF_VFD;
    // Bit mask
    GPIO_CONF_VFD.pin_bit_mask = (1ULL << SER_LEFT) | (1ULL << SER_MID) | (1ULL << SER_RIGHT) |
                                 (1ULL << SRCLR) | (1ULL << SRCLK) | (1ULL << RCLK) |
                                 (1ULL << DRIVER1) | (1ULL << DRIVER2);
    // Pin mode
    GPIO_CONF_VFD.mode = GPIO_MODE_OUTPUT;
    // No pull-up/pull-down resistors.
    GPIO_CONF_VFD.pull_up_en = GPIO_PULLUP_DISABLE;
    GPIO_CONF_VFD.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // No interrupt enabled.
    GPIO_CONF_VFD.intr_type = GPIO_INTR_DISABLE;
    // Configure the GPIO with the settings.
    gpio_config(&GPIO_CONF_VFD);


    vfd_info();
    vfd_clear();

    gpio_set_level(RCLK, 0);
    gpio_set_level(SRCLK, 0);
    gpio_set_level(SRCLR, 1);

}



void vfd_update(uint8_t value){

    for(uint8_t i = 0; i < sizeof(value) * 8; i++){

        gpio_set_level(SER_MID, value & 1); // Write LSB of value to SER

		value >>= 1;

		gpio_set_level(SRCLK, 1); // Tick the shift register storage
		gpio_set_level(SRCLK, 0);
    }

    gpio_set_level(RCLK, 1); // Send SR storage to output
	gpio_set_level(RCLK, 0);
}



void vfd_value(uint8_t value, bool digit_select){
    
    static const uint8_t reg_values[10] = {

        //ABCDEFGH
        0b11101110, // 0
        0b00100100, // 1
        0b10111010, // 2
        0b10110110, // 3
        0b01110100, // 4
        0b11010110, // 5
        0b11011110, // 6
        0b10100100, // 7
        0b11111110, // 8
        0b11110110  // 9
    };

    // Digit mux - DRIVER1 = tens, DRIVER2 = ones
    if(digit_select == 0){
        gpio_set_level(DRIVER1, 0);
        gpio_set_level(DRIVER2, 1);

        vfd_update(reg_values[value % 10]); // Extract ones
    }
    else{
        gpio_set_level(DRIVER1, 1);
        gpio_set_level(DRIVER2, 0);

        vfd_update(reg_values[value / 10]); // Extract tens
    }
    
}



/* Update shift register */
void vfd_update_str(uint8_t *value_array){

    for(uint8_t i = 0; i < sizeof(value_array[0]) * 8; i++){

        gpio_set_level(SER_LEFT, value_array[0] & 1); // Write LSB of value to SER
        gpio_set_level(SER_MID, value_array[1] & 1);
        gpio_set_level(SER_RIGHT, value_array[2] & 1);

        for (uint8_t j = 0; j < 3; j++){
            value_array[j] >>= 1;
        }

        gpio_set_level(SRCLK, 1); // Tick the shift register storage
		gpio_set_level(SRCLK, 0);
    }

    gpio_set_level(RCLK, 1); // Send SR storage to output
	gpio_set_level(RCLK, 0);
}


/* Display a string */
void vfd_value_str(const char *input_str, bool digit_select){
    
    static const uint8_t reg_values[10] = {

        //ABCDEFGH
        0b11101110, // 0
        0b00100100, // 1
        0b10111010, // 2
        0b10110110, // 3
        0b01110100, // 4
        0b11010110, // 5
        0b11011110, // 6
        0b10100100, // 7
        0b11111110, // 8
        0b11110110  // 9
    };

    

    size_t len = strlen(input_str);

    // We’ll assume max 2 digits since you’re using DRIVER1 + DRIVER2
    // If you want more digits, expand with more DRIVER pins and loops.
    if(len > 2) len = 2; 

    for(size_t i = 0; i < len; i++) {
        char c_left = input_str[len - 5 - i]; // rightmost digit first
        char c_mid = input_str[len - 3 - i]; // rightmost digit first
        char c_right = input_str[len - 1 - i]; // rightmost digit first

        // if(c < '0' || c > '9'){
        //     continue; // skip non-digits
        // } 

        uint8_t digit_left = c_left - '0';
        uint8_t digit_mid = c_mid - '0';
        uint8_t digit_right = c_right - '0';

        if(i == 0) { // ones place
            gpio_set_level(DRIVER1, 0);
            gpio_set_level(DRIVER2, 1);
        
        } else {     // tens place
            gpio_set_level(DRIVER1, 1);
            gpio_set_level(DRIVER2, 0);

        }

        uint8_t output_array[3] = {
            reg_values[digit_left],
            reg_values[digit_mid],
            reg_values[digit_right]
        };

        vfd_update_str(output_array);
    }
    
}



void vfd_clear(void){
    gpio_set_level(SRCLR, 0); // Clear the shift register once
    gpio_set_level(SRCLR, 1);
}