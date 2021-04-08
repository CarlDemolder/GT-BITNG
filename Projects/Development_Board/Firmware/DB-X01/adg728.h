#ifndef __ADG728_H
#define __ADG728_H

#include "serial_slave.h"
#include "i2c.h"

#if ADG728

/* Device Functionality
A0 = GND & A1 = GND => sets the second least and least bit of the 7 bit slave address.
Each bit in the serial word corresponds to a switch state.
Logic 1 = Switch Closed
Logic 0 = Switch Open

On power-up, all switches will be in the OFF condition and the internal shift register is filled with zeros and will remain so until a valid
write takes place.

Slave Address: x10011XX = > XX correspond to the selection of A0 and A1.   
/* String literals stored in enums */

enum ADG728_REGISTER_CONSTANTS
{
    ADG728_SLAVE_ADDRESS = 0X4C,
    ADG728_SWITCH_1 = 0X01,
    ADG728_SWITCH_2 = 0X02,
    ADG728_SWITCH_3 = 0X04,
    ADG728_SWITCH_4 = 0X08,
    ADG728_SWITCH_5 = 0X10,
    ADG728_SWITCH_6 = 0X20,
    ADG728_SWITCH_7 = 0X40,
    ADG728_SWITCH_8 = 0X80,
};

/* Control Struct */

struct ADG728_Control_Struct
{
    ret_code_t error_code;                                  /**< Variable to track errors */
    uint8_t slave_address;                                  /**< Variable to record the I2C slave address */

    uint8_t analog_switch;                                  /**< Variable to record which switch to open/close */
         
    uint8_t i2c_data[1];                                    /**< Variable to record the data communicated through I2C */
    uint8_t register_byte_count;                            /**< Variable to record the number of bytes communicated through I2C */

};

/* Register Structs */


/* @brief Device ID Register. This read-only register contains the device ID.
* Register address: 0x0F
* D[15:12] Rev = Indicates the revision number
* D[11:0] DID = Indicates the Device ID
*/
struct ADG728_Device_ID_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t rev;                                  /**< Value to store the revision number */
    uint8_t device_id[2];                         /**< Array to store the Device ID */
};


/* Public Functions */

void adg728_init(void);

void adg728_close_switch(uint8_t analog_switch);

/* Static Functions */

// No static functions to declare

#endif

#endif // __ADG728_H