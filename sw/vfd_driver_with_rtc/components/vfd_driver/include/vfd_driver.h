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

// Functions
/* Display info about the driver */
void vfd_info(void); 

/* Initialize driver */
void vfd_init(void); 

/* Update shift register */
void vfd_update(uint8_t value);

/* Display a value */
void vfd_value(uint8_t value, bool digit_select);

/* Clears the shift register storage*/
void vfd_clear(void);













#endif