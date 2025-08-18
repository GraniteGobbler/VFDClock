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






































































void vfd_update_str(uint8_t left_bits, uint8_t mid_bits, uint8_t right_bits){

    for(uint8_t i = 0; i < sizeof(left_bits) * 8; i++){

        gpio_set_level(SER_LEFT, left_bits & 1); // Write LSB of value to SER
        gpio_set_level(SER_MID, mid_bits & 1);
        gpio_set_level(SER_RIGHT, right_bits & 1);

        left_bits >>= 1;
        mid_bits >>= 1;
        right_bits >>= 1;

        gpio_set_level(SRCLK, 1); // Tick the shift register storage
		gpio_set_level(SRCLK, 0);
    }

    gpio_set_level(RCLK, 1); // Send SR storage to output
	gpio_set_level(RCLK, 0);
}



void vfd_value_str(const char *input_str, bool position){
    
    static const uint8_t number_map[10] = {

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

    static const uint8_t character_map[26] = {

        //ABCDEFGH
        0b11111100,  // A
        0b01011110,  // B
        0b11001010,  // C
        0b00111110,  // D
        0b11011010,  // E
        0b11011000,  // F
        0b11001110,  // G
        0b01111100,  // H
        0b01001000,  // I
        0b00101110,  // J
        0b11011100,  // K
        0b01001010,  // L
        0b10001100,  // M
        0b11101100,  // N
        0b00011110,  // O
        0b11111000,  // P
        0b11110100,  // Q
        0b00011000,  // R
        0b11010110,  // S same as 5
        0b01011010,  // T
        0b00001110,  // U
        0b01101110,  // V
        0b01100010,  // W
        0b01111100,  // X same as H
        0b01110110,  // Y
        0b10111010,  // Z same as 2
    };

 

    for(uint8_t i = 0; i < 6; i++){
        char character = input_str[i/2 + (position)];
                            // Digit mux - DRIVER1 = tens, DRIVER2 = ones
        if(position == 0){  // 0 = left position, 1 = right position
            gpio_set_level(DRIVER1, 1);
            gpio_set_level(DRIVER2, 0);
        }
        else{
            gpio_set_level(DRIVER1, 0);
            gpio_set_level(DRIVER2, 1);
        }


        if (character >= '0' && character <= '9'){
            vfd_update(number_map[character - '0']);
        }
        else if (character >= 'a' && character <= 'z'){
            vfd_update(character_map[character - 'a']);
        }
        else if (character >= 'A' && character <= 'Z'){
            vfd_update(character_map[character - 'A']);
        }
    }  
}


void vfd_clear(void){
    gpio_set_level(SRCLR, 0); // Clear the shift register once
    gpio_set_level(SRCLR, 1);
}