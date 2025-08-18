#ifndef vfd_driver
#define vfd_driver

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

// Includes
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"


// Pin Mapping
#define SER_LEFT    GPIO_NUM_47
#define SER_MID     GPIO_NUM_38
#define SER_RIGHT   GPIO_NUM_21

#define SRCLR       GPIO_NUM_35
#define SRCLK       GPIO_NUM_36
#define RCLK        GPIO_NUM_37

#define DRIVER1     GPIO_NUM_11 //Left Digit pin for muxing
#define DRIVER2     GPIO_NUM_12 //Right Digit pin for muxing

extern bool mux_select;
extern uint32_t mux_cnt;

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
        0b01011110,  // b
        0b11001010,  // C
        0b00111110,  // d
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
        0b00011110,  // o
        0b11111000,  // P
        0b11110100,  // Q
        0b00011000,  // r
        0b11010110,  // S same as 5
        0b01011010,  // t
        0b00001110,  // u
        0b01101110,  // V
        0b01100010,  // W
        0b01111100,  // X same as H
        0b01110110,  // Y
        0b10111010,  // Z same as 2
    };

// Functions
/* Display info about the driver */
void vfd_info(void); 

/* Initialize driver */
void vfd_init(void); 

/* Update shift register */
void vfd_update(uint8_t value);

/* Display a number */
void vfd_value(uint8_t value, bool digit_select);

/* Update shift register */
void vfd_update_str(uint32_t value);

/* Display a string */
void vfd_value_str(const char *input_str, bool digit_select);

/* Clears the shift register storage*/
void vfd_clear(void);













#endif