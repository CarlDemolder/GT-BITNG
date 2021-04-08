#include "ADG728.h"

#if ADG728

static struct ADG728_Control_Struct control;

/* Public Functions */

void adg728_init(void)
{
    NRF_LOG_INFO("adg728_init");
    control.slave_address = ADG728_SLAVE_ADDRESS;
}

void adg728_close_switch(uint8_t analog_switch)
{
    NRF_LOG_INFO("adg728_uninit");
    
    switch(analog_switch)
    {
        case 1:
            NRF_LOG_INFO("Close switch #1");
            control.i2c_data[0] = ADG728_SWITCH_1;
            break;

        case 2:
            NRF_LOG_INFO("Close switch #2");
            control.i2c_data[0] = ADG728_SWITCH_2;
            break;

        case 3:
            NRF_LOG_INFO("Close switch #3");
            control.i2c_data[0] = ADG728_SWITCH_3;
            break;

        case 4:
            NRF_LOG_INFO("Close switch #4");
            control.i2c_data[0] = ADG728_SWITCH_4;
            break;

        case 5:
            NRF_LOG_INFO("Close switch #5");
            control.i2c_data[0] = ADG728_SWITCH_5;
            break;

        case 6:
            NRF_LOG_INFO("Close switch #6");
            control.i2c_data[0] = ADG728_SWITCH_6;
            break;

        case 7:
            NRF_LOG_INFO("Close switch #7");
            control.i2c_data[0] = ADG728_SWITCH_7;
            break;

        case 8:
            NRF_LOG_INFO("Close switch #8");
            control.i2c_data[0] = ADG728_SWITCH_8;
            break;

        default:
            NRF_LOG_INFO("Default: All Switches Open");
            control.i2c_data[0] = 0x00;
            break;
    }
    i2c_no_stop_write_register(control.slave_address, control.i2c_data, 1);
}

/* Static Functions */


#endif