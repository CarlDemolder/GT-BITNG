#include "max30003.h"

#if MAX30003

static struct MAX30003_Status_Register_Struct status_register;
static struct MAX30003_Control_Struct control;
static struct MAX30003_Interrupt_Register_Struct interrupt1_register;
static struct MAX30003_Interrupt_Register_Struct interrupt2_register;
static struct MAX30003_Interrupt_Manager_Register_Struct interrupt_manager_register;
static struct MAX30003_Dynamic_Mode_Manager_Register_Struct dynamic_mode_manager_register;
static struct MAX30003_Info_Register_Struct info_register;
static struct MAX30003_General_Configuration_Register_Struct general_configuration_register;
static struct MAX30003_Calibration_Configuration_Register_Struct calibration_configuration_register;
static struct MAX30003_Input_Multiplexer_Configuration_Register_Struct input_multiplexer_configuration_register;
static struct MAX30003_ECG_Configuration_Register_Struct ecg_configuration_register;
static struct MAX30003_RTOR1_Configuration_Register_Struct rtor1_configuration_register;
static struct MAX30003_RTOR2_Configuration_Register_Struct rtor2_configuration_register;
static struct MAX30003_ECG_FIFO_Memory_Register_Struct ecg_fifo_memory_register;
static struct MAX30003_Software_Reset_Register_Struct software_reset_register;
static struct MAX30003_Synchronization_Register_Struct synchronization_register;
static struct MAX30003_FIFO_Reset_Register_Struct fifo_reset_register;


/* Public Functions */

void max30003_init(void) 
{
    NRF_LOG_INFO("max30003_init");

    control.register_byte_count = 3;    // Number of bytes per register
    control.long_term_storage = 0;      // Initially set the long term data storage off
    control.samples_per_second = 128;
    control.bytes_per_sample = 2;       // Bytes per sample
    control.counter = 0;                                                  // Counter to iterate through measurements taken

    control.current_sample_count = 0;    
    control.interrupt = 0;    // Disabling the interrupt flag for ECG Data
    control.data_ready_for_transmit = 0;                    // Data ready for transmit
    control.bytes_left_to_transmit = 0;

    control.samples_per_interrupt = max30003_get_samples_per_interrupt();
    NRF_LOG_INFO("MAX30003: samples_per_interrupt: %u", control.samples_per_interrupt);

    NRF_LOG_INFO("MAX30003: samples_per_second: %u", control.samples_per_second);
    control.samples_per_recording_session = 3600 * control.samples_per_second;         // Each recording session is 1 hour
    NRF_LOG_INFO("MAX30003: samples_per_recording_session: %u", control.samples_per_recording_session);

    NRF_LOG_INFO("MAX30003: bytes_per_sample: %u", control.bytes_per_sample);
    control.bytes_per_recording_session = control.samples_per_recording_session * control.bytes_per_sample;
    NRF_LOG_INFO("MAX30003: bytes_per_recording_session: %u", control.bytes_per_recording_session);

    control.bytes_per_bluetooth_transmission = bluetooth_get_bytes_per_transmission();
    control.samples_per_bluetooth_transmission = control.bytes_per_bluetooth_transmission/control.bytes_per_sample;
    NRF_LOG_INFO("control.samples_per_bluetooth_transmission: %u", control.samples_per_bluetooth_transmission);



    control.external_memory_start_address = MAX30003_EXTERNAL_MEMORY_START_ADDRESS;     // Start address of the external memory to store ECG data
    control.external_memory_end_address = MAX30003_EXTERNAL_MEMORY_END_ADDRESS;         // End address of the external memory to store ECG data
    
    control.external_memory_write_start_address = control.external_memory_start_address;            // Initializing the start address to the memory start address
    control.external_memory_write_current_address = control.external_memory_write_start_address;    // Initializing the current write address to the write start address

    control.external_memory_transmit_start_address = 0;             // The transmitting start address is set the write start address
    control.external_memory_transmit_end_address = 0;               // The transmitting end address is set to the write end address
    control.external_memory_transmit_current_address = 0;           // The transmitting current address is set to the transmit start address

    info_register.register_pointer = MAX30003_INFO;
    status_register.register_pointer = MAX30003_STATUS;
    interrupt1_register.register_pointer = MAX30003_EN_INT1;
    interrupt1_register.register_pointer = MAX30003_EN_INT2;
    interrupt_manager_register.register_pointer = MAX30003_MNGR_INT;
    dynamic_mode_manager_register.register_pointer = MAX30003_MNGR_DYN;

    general_configuration_register.register_pointer = MAX30003_CNFG_GEN;
    calibration_configuration_register.register_pointer = MAX30003_CNFG_CAL;
    input_multiplexer_configuration_register.register_pointer = MAX30003_CNFG_EMUX;
    ecg_configuration_register.register_pointer = MAX30003_CNFG_ECG;
    rtor1_configuration_register.register_pointer = MAX30003_CNFG_RTOR1;
    rtor2_configuration_register.register_pointer = MAX30003_CNFG_RTOR2;
    ecg_fifo_memory_register.register_pointer = MAX30003_ECG_FIFO_BURST;

    software_reset_register.register_pointer = MAX30003_SW_RS;
    synchronization_register.register_pointer = MAX30003_SYNCH;
    fifo_reset_register.register_pointer = MAX30003_FIFO_RST;

    max30003_soft_reset();              // Soft Reset
    max30003_init_pin_interrupt();      // Initializing the interrupt pin

    // Interrupt #2 Enable
    max30003_interrupt2_disable();

    // Interrupt Manager
    // MAX30003_MNGR_INT_ADDRESS, 0x78, 0x00, 0x04
    _max30003_read_interrupt_manager_register();
    interrupt_manager_register.efit = 0x1F;                           // DEFAULT: ECG FIFO INTERRUPT THRESHOLD = 32 WORDS
    interrupt_manager_register.clr_fast = 0x00;                       // DEFAULT: FAST MODE INTERRUPT CLEAR BEHAVIOR = FSTINT REMAINS ACTIVE UNTIL THE FAST MODE IS DISENGAGED
    interrupt_manager_register.clr_rrint = 0x00;                      // DEFAULT: R-R DETECT INTERRUPT CLEAR BEHAVIOR = CLEAR RRINT ON STATUS REGISTER READ BACK
    interrupt_manager_register.clr_samp = 0x01;                       // DEFAULT: SAMPLE SYNCHRONIZATION PULSE CLEAR BEHAVIOR = SELF CLEAR SAMP AFTER 1/4 OF 1 DATA RATE CYCLE
    interrupt_manager_register.samp_it = 0x00;                        // DEFAULT: SAMPLE SYNCHRONIZATION PULSE FREQUENCY = ISSUED EVERY SAMPLE INSTANT
    _max30003_write_interrupt_manager_register();
    _max30003_read_interrupt_manager_register();

    // Dynamic Mode Manager
    // MAX30003_MNGR_DYN_ADDRESS, 0x3F, 0x00, 0x00
    _max30003_read_dynamic_mode_manager_register();
    dynamic_mode_manager_register.fast = 0x00;                        // DEFAULT: ECG CHANNEL FAST RECOVERY MODE SELECTION = NORMAL MODE (FAST RECOVERY MODE DISABLED)
    dynamic_mode_manager_register.fast_th = 0x3F;                     // DEFAULT: AUTOMATIC FAST RECOVERY THRESHOLD = (DISABLED DUE TO FAST)
    _max30003_write_dynamic_mode_manager_register();
    _max30003_read_dynamic_mode_manager_register();

    // General Configuration
    // MAX30003_CNFG_GEN_ADDRESS, 0x08, 0x10, 0x07
    _max30003_read_general_configuration_register();
    general_configuration_register.en_ulp_lon = 0x00;                 // DEFAULT: ULTRA-LOW POWER LEAD-ON DETECTION ENABLE = ULP LEAD-ON DETECTION DISABLED
    general_configuration_register.fmstr = 0x00;                      // DEFAULT: MASTER CLOCK FREQUENCY = 32768 Hz
    general_configuration_register.en_ecg = 0x01;                     // ECG CHANNEL ENABLE = 1
    general_configuration_register.en_dcloff = 0x01;                  // DC LEAD-OFF DETECTION ENABLE = DCLOFF DETECTION APPLIED TO ECGP/N PINS 
    general_configuration_register.dcloff_ipol = 0x00;                // DEFAULT: DC LEAD-OFF CURRENT POLARITY = 0 (ECGP-PULLUP & ECGN-PULLDOWN)
    general_configuration_register.dcloff_imag = 0x00;                // DEFAULT: DC LEAD-OFF CURRENT MAGNITUDE SELECTION = 0 nA
    general_configuration_register.dcloff_vth = 0x00;                 // DEFAULT: DC LEAD-OFF VOLTAGE THRESHOLD SELECTION = VMID +/- 300 mV
    general_configuration_register.en_rbias = 0x00;                   // DEFAULT: ENABLE AND SELECT RESISTIVE LEAD BIAS MODE = RESISTIVE BIAS DISABLED
    general_configuration_register.rbiasv = 0x01;                     // RESISTIVE BIAS MODE VALUE SELECTION = 100 M OHM
    general_configuration_register.rbiasp = 0x01;                     // RESISTIVE BIAS ON POSITIVE INPUT = 1
    general_configuration_register.rbiasn = 0x01;                     // RESISTIVE BIAS ON NEGATIVE INPUT = 1
    _max30003_write_general_configuration_register();
    _max30003_read_general_configuration_register();

    // Calibration Configuration
    // MAX30003_CNFG_CAL_ADDRESS, 0x70, 0x48, 0x00
    _max30003_read_calibration_configuration_register();
    calibration_configuration_register.en_vcal = 0x01;                // CALIBRATION SOURCE (VCALP & VCALN) ENABLE = 1
    calibration_configuration_register.vmode = 0x01;                  // CALIBRATION SOURCE MODE SELECTION = BIPOLAR
    calibration_configuration_register.vmag = 0x01;                   // CALIBRATION SOURCE MAGNITUDE SELECTION = 0.50 mV
    calibration_configuration_register.fcal = 0x04;                   // DEFAULT: CALIBRATION SOURCE FREQUENCY SELECTION = 1 Hz
    calibration_configuration_register.fifty = 0x01;                  // DEFAULT: CALIBRATION SOURCE DUTY CYCLE MODE SELECTION = 1 (THIGH = 50%; THIGH[10:0] IGNORED)
    calibration_configuration_register.thigh = 0x0000;                // DEFAULT: CALIBRATION SOURCE TIME HIGH SELECTION = 0 (VALUE IS IGNORED IF FIFTY = 1)
    _max30003_write_calibration_configuration_register();
    _max30003_read_calibration_configuration_register();
    
    // EMUX Configuration
    // MAX30003_CNFG_EMUX_ADDRESS, 0x0B, 0x00, 0x00
    _max30003_read_input_multiplexer_configuration_register();
    input_multiplexer_configuration_register.pol = 0x00;              // DEFAULT: ECG INPUT POLARITY SELECTION = NON-INVERTED
    input_multiplexer_configuration_register.openp = 0x00;            // ECGP INPUT SWITCH = ECGP IS INTERNALLY CONNECTED TO ECG AFE CHANNEL
    input_multiplexer_configuration_register.openn = 0x00;            // ECGN INPUT SWITCH = ECGN IS INTERNALLY CONNECTED TO ECG AFE CHANNEL
    input_multiplexer_configuration_register.calp_sel = 0x02;         // ECGP CALIBRATION SELECTION = INPUT CONNECTED TO VCALP
    input_multiplexer_configuration_register.caln_sel = 0x03;         // ECGN CALIBRATION SELECTION = INPUT CONNECTED TO VCALN
    _max30003_write_input_multiplexer_configuration_register();
    _max30003_read_input_multiplexer_configuration_register();

    // ECG Configuration
    // MAX30003_CNFG_ECG_ADDRESS, 0x80, 0x50, 0x00
    _max30003_read_ecg_configuration_register();
    ecg_configuration_register.rate = 0x02;                           // DEFAULT: ECG DATA RATE = 128 SPS
    ecg_configuration_register.gain = 0x00;                           // DEFAULT: ECG CHANNEL GAIN SETTING = 20 V/V
    ecg_configuration_register.dhpf = 0x01;                           // DEFAULT: ECG CHANNEL DIGITAL HIGH-PASS FILTER CUTOFF FREQUENCY = 0.50 Hz
    ecg_configuration_register.dlpf = 0x01;                           // DEFAULT: ECG CHANNEL DIGITAL LOW-PASS FILTER CUTOFF FREQUENCY = 40 Hz
    _max30003_write_ecg_configuration_register();
    _max30003_read_ecg_configuration_register();

    // R-R #1 Configuration => R-R DETECTION DISABLED
    // MAX30003_CNFG_RTOR1_ADDRESS, 0x3F, 0x23, 0x00
    _max30003_read_rtor1_configuration_register();
    rtor1_configuration_register.wndw = 0x03;                         // DEFAULT: WIDTH OF THE AVERAGING WINDOW = 12 * 8 ms
    rtor1_configuration_register.gain = 0x0F;                         // DEFAULT: R-R GAIN = AUTOSCALE (DEFAULT)
    rtor1_configuration_register.en_rtor = 0x00;                      // DEFAULT: ECG RTOR DTECTION ENABLE = 0
    rtor1_configuration_register.pavg = 0x02;                         // DEFAULT: R-R PEAK AVERAGING WEIGHT FACOTR = 8
    rtor1_configuration_register.ptsf = 0x03;                         // DEFAULT: R-R PEAK THRESHOLD SCALING FACTOR = 4/8
    _max30003_write_rtor1_configuration_register();
    _max30003_read_rtor1_configuration_register();

    // R-R #2 Configuration => R-R DETECTION DISABLED
    // MAX30003_CNFG_RTOR2_ADDRESS, 0x20, 0x24, 0x00
    _max30003_read_rtor2_configuration_register();
    rtor2_configuration_register.hoff = 0x20;                         // DEFAULT: t_HOLD_OFF_MIN = 32 * 8 ms, 
    rtor2_configuration_register.ravg = 0x02;                         // DEFAULT: R-R INTERVAL AVERAGING WEIGHT FACTOR = 8
    rtor2_configuration_register.rhsf = 0x04;                         // DEFAULT: R-R INTERVAL HOLD OFF SCALING FACTOR = 4/8
    _max30003_write_rtor2_configuration_register();
    _max30003_read_rtor2_configuration_register();

    control.samples_per_interrupt = interrupt_manager_register.efit + 1;  // Setting the default Samples recored per interrupt
    control.bytes_per_interrupt = control.samples_per_interrupt * control.bytes_per_sample;
    NRF_LOG_INFO("control.samples_per_interrupt: %u", control.samples_per_interrupt);
}

void max30003_read_device_info(void) 
{
    NRF_LOG_INFO("max30003_read_device_info");
    _max30003_spim_read_registers(info_register.register_pointer, control.spi_data, control.register_byte_count);

    NRF_LOG_INFO("Device Information:");
    NRF_LOG_HEXDUMP_INFO(control.spi_data, control.register_byte_count); // Hex Dump Output (temp)
}

void max30003_read_device_status(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Status \r\n");

    _max30003_spim_read_registers(status_register.register_pointer, control.spi_data, control.register_byte_count);

    NRF_LOG_INFO("Device Status: \r\n");
    NRF_LOG_HEXDUMP_INFO(control.spi_data, control.register_byte_count); // Hex Dump Output (temp)
}

void max30003_set_samples_per_interrupt(uint8_t temp_samples_per_interrupt)
{
    NRF_LOG_INFO("max30003_set_samples_per_interrupt");
    if(temp_samples_per_interrupt > MAX30003_MAX_FIFO_WORDS)
    {
        control.samples_per_interrupt = MAX30003_MAX_FIFO_WORDS;
    }
    else
    {
        control.samples_per_interrupt = temp_samples_per_interrupt;
    }
    control.bytes_per_interrupt = control.samples_per_interrupt * control.bytes_per_sample;
    interrupt_manager_register.efit = control.samples_per_interrupt - 1;
    _max30003_write_interrupt_manager_register();
    _max30003_read_interrupt_manager_register();
}

void max30003_set_samples_per_second(uint16_t temp_samples_per_second)
{
    NRF_LOG_INFO("max30003_set_samples_per_second");
    switch(temp_samples_per_second)
    {
        case MAX30003_SPS_128:
            control.samples_per_second = MAX30003_SPS_128;
            ecg_configuration_register.rate = 0x02;                           // DEFAULT: ECG DATA RATE = 128 SPS
            return;

        case MAX30003_SPS_256:
            control.samples_per_second = MAX30003_SPS_256;
            ecg_configuration_register.rate = 0x01;
            return;

        case MAX30003_SPS_512:
            control.samples_per_second = MAX30003_SPS_512;
            ecg_configuration_register.rate = 0x00;
            return;

        default:
            control.samples_per_second = MAX30003_SPS_128;
            ecg_configuration_register.rate = 0x02;                           // DEFAULT: ECG DATA RATE = 128 SPS
            return;
    }
    _max30003_write_ecg_configuration_register();
    _max30003_read_ecg_configuration_register();
}

void max30003_start_recording(void)
{
    NRF_LOG_INFO("max30003_start_recording");
    _max30003_write_fifo_reset_register();           // Reset the FIFO to clear the FIFO
    max30003_interrupt1_enable();                   // Interrupt #1 Enable
    max30003_enable_pin_interrupt();                // Enable the interrupt of the NRF52
    max30003_sync();                                // Synchronize the clock frequency of the MAX30003 with the NRF52
}

uint8_t max30003_get_samples_per_interrupt(void)
{
    NRF_LOG_INFO("max30003_get_samples_per_interrupt");
    return control.samples_per_interrupt;
}

uint8_t max30003_get_samples_per_second(void)
{
    NRF_LOG_INFO("max30003_get_samples_per_second");
    return control.samples_per_second;
}

uint8_t max30003_get_bytes_per_sample(void)
{
    NRF_LOG_INFO("max30003_get_bytes_per_sample");
    return control.bytes_per_sample;
}

uint8_t max30003_get_bytes_per_interrupt(void)
{
    NRF_LOG_INFO("max30003_get_bytes_per_interrupt");
    return control.bytes_per_interrupt;
}

void max30003_interrupt1_enable(void)
{
    NRF_LOG_INFO("max30003_interrupt1_enable");
    _max30003_read_interrupt1_register();
    interrupt1_register.en_eint = 1;        // Enable Interrupt when FIFO Buffer has been filled
    interrupt1_register.int1b_port = 3;     // Open-Drain nMOS Driver with Internal 125k Ohm pullup resistance
    _max30003_write_interrupt1_register();
    _max30003_read_interrupt1_register();    // Verify Register
}

void max30003_interrupt1_disable(void)
{
    NRF_LOG_INFO("max30003_interrupt1_disable");
    _max30003_read_interrupt1_register();
    interrupt1_register.en_eint = 0;        // Disable Interrupt when FIFO Buffer has been filled
    interrupt1_register.int1b_port = 0;     // Disabled (Three-state)
    _max30003_write_interrupt1_register();
    _max30003_read_interrupt1_register();    // Verify Register
}

void max30003_interrupt2_enable(void)
{
    NRF_LOG_INFO("max30003_interrupt2_enable");
    _max30003_read_interrupt2_register();
    interrupt2_register.en_eovf = 1;        // 
    interrupt2_register.int1b_port = 3;     // Open-Drain nMOS Driver with Internal 125k Ohm pullup resistance
    _max30003_write_interrupt2_register();
    _max30003_read_interrupt2_register();    // Verify Register
}

void max30003_interrupt2_disable(void)
{
    NRF_LOG_INFO("max30003_interrupt2_disable");
    _max30003_read_interrupt2_register();
    interrupt2_register.en_eovf = 0;        // Disable Interrupt when ECG FIFO Overflow has occured
    interrupt2_register.int1b_port = 0;     // Disabled (Three-state)
    _max30003_write_interrupt2_register();
    _max30003_read_interrupt2_register();    // Verify Register
}

/* 
* Read ECG FIFO Memory Register to read the following values:
* D[23:6], ECG_VOLTAGE = ECG Sample Voltage Data; 18 Bit ECG Voltage Information at the Requested Sample Rate in Left Justified Two's Complement Format
* D[5:3], ETAG = DATA Tag; Data condition Tag
* D[2:0], PTAG = ???
*/
void max30003_read_ecg_fifo_memory(void)
{
    NRF_LOG_INFO("max30003_read_ecg_fifo_memory_register");
    NRF_LOG_INFO("control.samples_per_interrupt: %u", control.samples_per_interrupt);
    _max30003_spim_read_registers(ecg_fifo_memory_register.register_pointer, ecg_fifo_memory_register.data, control.samples_per_interrupt*3);
    uint32_t temp_ecg_voltage = 0;
    control.counter = 0;
    for(uint8_t i = 0; i < control.samples_per_interrupt*3; i=i+3)
    {
        temp_ecg_voltage = 0;
        temp_ecg_voltage = (ecg_fifo_memory_register.data[i+2] & 0b11000000) >> 6;
        temp_ecg_voltage = temp_ecg_voltage | (((uint32_t) ecg_fifo_memory_register.data[i+1]) << 2);
        temp_ecg_voltage = temp_ecg_voltage | (((uint32_t) ecg_fifo_memory_register.data[i]) << 10); 
//        NRF_LOG_INFO("temp_ecg_voltage: %X", temp_ecg_voltage);
        ecg_fifo_memory_register.ecg_voltage[control.counter] = (uint16_t)((temp_ecg_voltage & 0b00000000000000111111111111111100)>>2);
        ecg_fifo_memory_register.etag[control.counter] = (ecg_fifo_memory_register.data[i+2] & 0b00111000) >> 3;
        ecg_fifo_memory_register.ptag[control.counter] = ecg_fifo_memory_register.data[i+2] & 0b00000111;
        NRF_LOG_INFO("W: %u, R:%X %X %X, V: %X", (control.counter+1), ecg_fifo_memory_register.data[i], ecg_fifo_memory_register.data[i+1], 
        ecg_fifo_memory_register.data[i+2], ecg_fifo_memory_register.ecg_voltage[control.counter]);
        NRF_LOG_INFO("ETAG: %X, PTAG: %X", ecg_fifo_memory_register.etag[control.counter], ecg_fifo_memory_register.ptag[control.counter]);
        NRF_LOG_INFO("");
        control.counter++;
    }
    NRF_LOG_INFO("Counter: %u", control.counter);
}

void max30003_get_ecg_voltage(uint8_t* temp_data, uint8_t temp_data_length)
{
    NRF_LOG_INFO("max30003_get_ecg_voltage");
    for(int i = 0; i < temp_data_length; i++)
    {
        temp_data[i] = ecg_fifo_memory_register.ecg_voltage[i];
    }
}

void max30003_soft_reset(void) 
{
    NRF_LOG_INFO("max_30003_soft_reset");
    _max30003_write_software_reset_register();
    nrf_delay_ms(MAX30003_DELAY); // Delay the MCU to allow the MAX30003 to go through a soft reset
}

void max30003_sync(void) 
{
    NRF_LOG_INFO("max30003_sync");
    _max30003_write_synchronize_register();
}

uint8_t max30003_get_status_register_eovf(void)
{
    _max30003_read_status_register();
    return status_register.eovf;
}

void max30003_init_pin_interrupt(void)
{
    NRF_LOG_INFO("max30003_init_pin_interrupt");
    nrf_gpio_pin_dir_set(MAX30003_INT1_PIN, NRF_GPIO_PIN_DIR_INPUT);
    nrfx_gpiote_in_config_t max30003_interrupt_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    ret_code_t error_code = nrfx_gpiote_in_init(MAX30003_INT1_PIN, &max30003_interrupt_config, max30003_pin_interrupt_handler);
    
    NRF_LOG_INFO(" nrfx_gpiote_in_init: %d: \r\n", error_code);
    APP_ERROR_CHECK(error_code);
}

void max30003_enable_pin_interrupt(void)
{
    NRF_LOG_INFO("max30003_enable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, true);
    nrf_delay_ms(MAX30003_DELAY);
}

void max30003_disable_pin_interrupt(void)
{
    NRF_LOG_INFO("max30003_disable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, false);
}

void max30003_pin_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("max30003_interrupt_handler");
    UNUSED_PARAMETER(pin);
    UNUSED_PARAMETER(action);

    max30003_interrupt_handler();    // Call to Overall ECG Manager to manage ECG workflow
}

void max30003_fifo_reset(void)
{
    NRF_LOG_INFO("max30003_fifo_reset");
    _max30003_write_fifo_reset_register();
}

void max30003_interrupt_handler(void)
{
    NRF_LOG_INFO("ecg_interrupt_handler");
    
    if(control.interrupt == 1)
    {
        uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
        state_handler(spim_enable_command); // Enable SPIM Module

        uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
        state_handler(spim_init_command); // Initialize SPIM Module

        uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
        state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to MAX30003 for the SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        if(max30003_get_status_register_eovf())   // Check to see if one needs to issue a fifo reset to clear the cache
        {
            max30003_fifo_reset();
        }
        else
        {
            max30003_read_ecg_fifo_memory();    // Read data from FIFO memory stored on MAX30003
            
            if(control.long_term_storage == 1)    
            {
                #if CY15B108QI
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                // Data is stored as a uint16_t data type. In order to save it to external memory, it is converted to a uinnt8_t data type.
                uint8_t cy15b108qi_voltage_data[control.bytes_per_interrupt]; // uint8_t array has twice the number of elements as uint16_t 
                for(uint8_t i = 0; i < control.samples_per_interrupt; i++)
                {
                    cy15b108qi_voltage_data[2*i] = (uint8_t) ((0xFF00 & ecg_fifo_memory_register.ecg_voltage[i]) >> 8);
                    NRF_LOG_INFO("Sample: %u, cy15b108qi_voltage_array: %u", 2*i, cy15b108qi_voltage_data[2*i]);
                    cy15b108qi_voltage_data[2*i+1] = (uint8_t) (0x00FF & ecg_fifo_memory_register.ecg_voltage[i]);
                    NRF_LOG_INFO("Sample: %u, cy15b108qi_voltage_array: %u", 2*i+1, cy15b108qi_voltage_data[2*i+1]);
                }
        
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
                state_handler(spim_enable_command); // Enable SPIM Module

                uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
                state_handler(spim_init_command); // Initialize SPIM Module

                spim_select_cs_pin_command[4] = CY15B108QI_CS_PIN;
                state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                uint8_t cy15b108qi_exit_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
                state_handler(cy15b108qi_exit_deep_power_down_mode_command); // Exit the Deep power down mode

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
            
                if(control.external_memory_write_current_address > (control.external_memory_end_address - control.bytes_per_interrupt))
                {
                    control.external_memory_write_current_address = control.external_memory_start_address;
                }

                cy15b108qi_write_registers(cy15b108qi_voltage_data, control.bytes_per_interrupt, control.external_memory_write_current_address);
                control.external_memory_write_current_address += control.bytes_per_interrupt;

                control.current_sample_count += control.samples_per_interrupt;
                NRF_LOG_INFO("control.current_sample_count: %u", control.current_sample_count);

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */     

                uint8_t cy15b108qi_enter_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
                state_handler(cy15b108qi_enter_deep_power_down_mode_command); // Enter the Deep power down mode

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
                state_handler(spim_uninit_command); // Uninitialize SPIM Module

                uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
                state_handler(spim_disable_command); // Disable SPIM Module

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                if(control.current_sample_count >= control.samples_per_recording_session)
                {
                    // The transmitting start address is set the write start address      
                    control.external_memory_transmit_start_address = control.external_memory_write_start_address;           
                    
                    // The transmitting current address is set the external memory transmit start address
                    control.external_memory_transmit_current_address = control.external_memory_transmit_start_address;
                    
                    // The transmitting end address is set to the last write address
                    control.external_memory_transmit_end_address = control.external_memory_write_current_address - control.bytes_per_interrupt;           
      
                    // The external memory write start address is set to the external memory write_current address
                    control.external_memory_write_start_address = control.external_memory_write_current_address;

                    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                    // Reset sampling count
                    control.current_sample_count = 0;

                    // Flag to state that the data is ready to be transmitted via BLE
                    control.data_ready_for_transmit = 1;  
                    
                    // Setting the number of bytes left to transmit to the number of bytes per recording session  
                    control.bytes_left_to_transmit = control.bytes_per_recording_session;

                    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                    uint8_t bluetooth_start_advertising_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_START_ADVERTISING_COMMAND};
                    state_handler(bluetooth_start_advertising_command); // Start to transmit the Recording Session of the ECG Data
                }
                #endif
            }
            else
            {
                uint8_t ble_max30003_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_INSTANT_ECG_CHAR_COMMAND};
                state_handler(ble_max30003_data); // Write the instant ECG data to GATT database
            }
        }
    }
}

void bluetooth_transmit_ecg_recording_session(void)
{
    NRF_LOG_INFO("bluetooth_transmit_recording_session");

}

void max30003_transmit_ecg_recording_session(void)
{
    NRF_LOG_INFO("max30003_transmit_recording_session"); 

    if(control.data_ready_for_transmit == 1)
    {
        uint8_t empty_values = 0;

        uint8_t bluetooth_ecg_data[control.bytes_per_bluetooth_transmission];

        while(control.bytes_left_to_transmit > 0)
        {
            #if CY15B108QI
            uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
            state_handler(spim_enable_command); // Enable SPIM Module

            uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
            state_handler(spim_init_command); // Initialize SPIM Module

            uint8_t spim_set_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
            state_handler(spim_set_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
            uint8_t cy15b108qi_exit_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
            state_handler(cy15b108qi_exit_deep_power_down_mode_command); // Exit the Deep power down mode

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            if((control.external_memory_transmit_current_address > (control.external_memory_end_address - control.bytes_per_bluetooth_transmission))
            && (control.bytes_left_to_transmit > control.bytes_per_bluetooth_transmission))
            {
                NRF_LOG_INFO("max30003_transmit_recording_session: case 1");
                // Get difference between end register and current transmit register
                uint8_t external_memory_end_address_difference = control.external_memory_end_address - control.external_memory_transmit_current_address; 
            
                // Creating an array to store the data until the end address
                uint8_t external_memory_end_address_data[external_memory_end_address_difference];

                // Read data from the registers until the last address
                cy15b108qi_fast_read_registers(external_memory_end_address_data, external_memory_end_address_difference, control.external_memory_transmit_current_address);
            
                // Reset the current read address
                control.external_memory_transmit_current_address = control.external_memory_start_address;

                // Calculate the rest of the data that is required to read
                uint8_t external_memory_start_address_difference = control.bytes_per_bluetooth_transmission - external_memory_end_address_difference;
            
                // Creating an array to store the data to fill the rest of the bluetooth buffer
                uint8_t external_memory_start_address_data[external_memory_start_address_difference];

                // Read data from the register until the number of bytes is read
                cy15b108qi_fast_read_registers(external_memory_start_address_data, external_memory_start_address_difference, control.external_memory_transmit_current_address);
            
                // Transfer the data from these registers to the Bluetooth data packet for transfer
            
                for(uint8_t i = 0; i < external_memory_end_address_difference; i++)
                {
                    bluetooth_ecg_data[i] = external_memory_end_address_data[i];
                }

                for(uint8_t i = 0; i < external_memory_start_address_difference; i++)
                {
                    bluetooth_ecg_data[i+external_memory_end_address_difference] = external_memory_start_address_data[i];
                }

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += external_memory_start_address_difference;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }
        
            else if((control.external_memory_transmit_current_address > (control.external_memory_end_address - control.bytes_left_to_transmit))
            && (control.bytes_left_to_transmit < control.bytes_per_bluetooth_transmission))
            {
                NRF_LOG_INFO("max30003_transmit_recording_session: case 2");
                // Get difference between end register and current transmit register
                uint8_t external_memory_end_address_difference = control.external_memory_end_address - control.external_memory_transmit_current_address; 
            
                // Creating an array to store the data until the end address
                uint8_t external_memory_end_address_data[external_memory_end_address_difference];

                // Read data from the registers until the last address
                cy15b108qi_fast_read_registers(external_memory_end_address_data, external_memory_end_address_difference, control.external_memory_transmit_current_address);
            
                // Reset the current read address
                control.external_memory_transmit_current_address = control.external_memory_start_address;

                // Calculate the rest of the data that is required to read
                uint8_t external_memory_start_address_difference = control.bytes_left_to_transmit - external_memory_end_address_difference;
            
                // Creating an array to store the data to fill the rest of the bluetooth buffer
                uint8_t external_memory_start_address_data[external_memory_start_address_difference];

                // Read data from the register until the number of bytes is read
                cy15b108qi_fast_read_registers(external_memory_start_address_data, external_memory_start_address_difference, control.external_memory_transmit_current_address);
            
                // Transfer the data from these registers to the Bluetooth data packet for transfer
            
                for(uint8_t i = 0; i < external_memory_end_address_difference; i++)
                {
                    bluetooth_ecg_data[i] = external_memory_end_address_data[i];
                }

                for(uint8_t i = 0; i < external_memory_start_address_difference; i++)
                {
                    bluetooth_ecg_data[i+external_memory_end_address_difference] = external_memory_start_address_data[i];
                }

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += external_memory_start_address_difference;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }

            else if((control.bytes_left_to_transmit < control.bytes_per_bluetooth_transmission) && 
            ((control.external_memory_transmit_current_address < (control.external_memory_end_address - control.bytes_left_to_transmit))))
            {
                NRF_LOG_INFO("max30003_transmit_recording_session: case 3");

                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
                NRF_LOG_INFO("control.bytes_per_bluetooth_transmission: %u", control.bytes_per_bluetooth_transmission);
                empty_values = (control.bytes_per_bluetooth_transmission - control.bytes_left_to_transmit);
                NRF_LOG_INFO("empty_value: %u", empty_values);

                // Read remaining data from external memory
                cy15b108qi_fast_read_registers(bluetooth_ecg_data, control.bytes_left_to_transmit, control.external_memory_transmit_current_address);
            
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }
            else
            {
                // Read data from external memory
                cy15b108qi_fast_read_registers(bluetooth_ecg_data, control.bytes_per_bluetooth_transmission, control.external_memory_transmit_current_address);
            
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }
        
            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            bluetooth_ecg_service_ecg_char_write(bluetooth_ecg_data);

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            if(control.bytes_left_to_transmit == 0)
            {
                NRF_LOG_INFO("There are no more bytes left to transmit.");

                uint8_t bluetooth_override_request_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_OVERRIDE_REQUEST_RECEIVED_COMMAND};
                state_handler(bluetooth_override_request_command); // Override the Request Received Flag to send message to BLE Client

                uint8_t bluetooth_transmit_ecg_recording_session_finished_command[7] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND,
                0x00, 0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TRANSMIT_ECG_DATA_FINISHED};
                state_handler(bluetooth_transmit_ecg_recording_session_finished_command); // Recording session finished
                
                control.data_ready_for_transmit = 0;
            }
    
            if(empty_values != 0)
            {
                NRF_LOG_INFO("There are empty values: %u", empty_values);
                
                uint8_t bluetooth_override_request_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_OVERRIDE_REQUEST_RECEIVED_COMMAND};
                state_handler(bluetooth_override_request_command); // Override the Request Received Flag to send message to BLE Client

                uint8_t bluetooth_transmit_ecg_data_empty_values_command[7] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND,
                0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TRANSMIT_ECG_DATA_EMPTY_VALUES, empty_values};
                state_handler(bluetooth_transmit_ecg_data_empty_values_command); // Send the number of empty values
            }
        }

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        control.data_ready_for_transmit = 0;    // Resetting the flag stating that data is not ready for transmit

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_enter_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_enter_deep_power_down_mode_command); // Enter the Deep power down mode

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
        state_handler(spim_uninit_command); // Uninitialize SPIM Module

        uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
        state_handler(spim_disable_command); // Disable SPIM Module  

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        #endif
    }
}

uint32_t max30003_get_bytes_left_to_transmit(void)
{
    NRF_LOG_INFO("max30003_get_bytes_left_to_transmit");
    return control.bytes_left_to_transmit;
}

void max30003_enable_long_term_storage(void)
{
    control.long_term_storage = 1;
}

void max30003_disable_long_term_storage(void)
{
    control.long_term_storage = 0;
}

void max30003_start_data_collection(void)
{
    NRF_LOG_INFO("max30003_start_data_collection");
    control.interrupt = 1;    // Enabling the interrupt

    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Select Chip Select Pin to CY15B108QI for the SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    if(control.long_term_storage == 1)
    {
        #if CY15B108QI
        uint8_t cy15b108qi_exit_hibernation_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_HIBERNATION_MODE_COMMAND};
        state_handler(cy15b108qi_exit_hibernation_command); // Exit the Hibernation power state

        uint8_t cy15b108qi_enter_deep_power_down_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_enter_deep_power_down_command); // Enter the deep-power-down power state
        #endif
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    spim_select_cs_pin_command[4] = MAX30003_CS_PIN;
    state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to MAX30003 for the SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    max30003_start_recording(); // Set the MAX30003 to start recording ECG data

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t bluetooth_override_request_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_OVERRIDE_REQUEST_RECEIVED_COMMAND};
    state_handler(bluetooth_override_request_command); // Override the Request Received Flag to send message to BLE Client

    uint8_t response_char_ecg_data_collection_started_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_ECG_DATA_COLLECTION_STARTED};
    state_handler(response_char_ecg_data_collection_started_command); // Sending message to phone that the recording session has started
}

void max30003_stop_data_collection(void)
{
    NRF_LOG_INFO("ecg_stop_data_recording");

    control.interrupt = 0;    // Disabling the interrupt
    control.long_term_storage = 0; // Disable the long term storage command

    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t max30003_disable_pin_interrupt_command[3] = {0x00, MAX30003_MODULE, MAX30003_DISABLE_PIN_INTERRUPT_COMMAND}; 
    state_handler(max30003_disable_pin_interrupt_command); // MAX30003: Disable Pin Interrupt Command

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to MAX30003 for the SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    max30003_disable_pin_interrupt(); // MAX30003: Disable Interrupt Command

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
    if(control.long_term_storage == 1)
    {
        #if CY15B108QI
        spim_select_cs_pin_command[4] = CY15B108QI_CS_PIN;
        state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

        uint8_t cy15b108qi_enter_hibernation_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND};
        state_handler(cy15b108qi_enter_hibernation_command);
        #endif
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t bluetooth_override_request_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_OVERRIDE_REQUEST_RECEIVED_COMMAND};
    state_handler(bluetooth_override_request_command); // Override the Request Received Flag to send message to BLE Client

    uint8_t response_char_ecg_data_collection_finished_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_ECG_DATA_COLLECTION_FINISHED};
    state_handler(response_char_ecg_data_collection_finished_command); // Sending message to phone that the recording session has started
}


/* Static Functions */


/* 
* Read Status Register to read the following values:
* D[23], EINT = ECG FIFO Interrupt, Active High; ECG records exceed the ECG FIFO Interrupt Threshold
* D[22], EOVF = ECG FIFO Overflow, Active High; ECG FIFO has overflown and the data record has been corrupted. FIFO Reset Command to reset signal
* D[21], FSTINT = ECG Fast Recovery Mode, Active High; Issued when ECG Fast Recovery Mode is engaged
* D[20], DCLOFFINT = DC Lead-Off Detection Interrupt, Active High; Remains active as long as the leads-off condition presists, then held until cleared 
* D[19] - D[12], NOT USED 
* D[11], LONINT = Ulta-Low Power (ULP) Leads-on Detection Interrupt; The MAX30003 has determined it is in a leads-on condition. 
* D[10], RRINT = ECG R to R Detector R Event Interrupt; Issued when the R to R detector has identified a new R event.
* D[9], SAMP = Sample Synchronization Pulse; Issued on the ECG base-rate sampling instant
* D[8], PLLINT = PLL Unlocked Interrupt; Indicates that the PLL has not yet achieved or has lost its phase lock. 
* D[7] - D[4], NOT USED
* D[3], LDOFF_PH = ECGP is above the high threshold (VTHH)
* D[2], LDOFF_PL = ECGP is below the low threshold (VTHL)
* D[1], LDOFF_NH = ECGN is above the high threshold (VTHH)
* D[0], LDOFF_NL = ECGN is below the low threshold (VTHL)
*/
static void _max30003_read_status_register(void)
{
    _max30003_spim_read_registers(status_register.register_pointer, control.spi_data, control.register_byte_count);

    status_register.eint = (control.spi_data[0] & 0b10000000) && 0b10000000;
    status_register.eovf = (control.spi_data[0] & 0b01000000) && 0b01000000;
    status_register.fstint = (control.spi_data[0] & 0b00100000) && 0b00100000;
    status_register.dcloffint = (control.spi_data[0] & 0b00010000) && 0b00010000;

    status_register.lonint = (control.spi_data[1] & 0b00001000) && 0b00001000; 
    status_register.rrint = (control.spi_data[1] & 0b00000100) && 0b00000100;
    status_register.samp = (control.spi_data[1] & 0b00000010) && 0b00000010;
    status_register.pllint = (control.spi_data[1] & 0b00000001) && 0b00000001;

    status_register.ldoff_ph = (control.spi_data[2] & 0b00001000) && 0b00001000;
    status_register.ldoff_pl = (control.spi_data[2] & 0b00000100) && 0b00000100;
    status_register.ldoff_nh = (control.spi_data[2] & 0b00000010) && 0b00000100;
    status_register.ldoff_nl = (control.spi_data[2] & 0b00000001) && 0b00000100;

    NRF_LOG_INFO("status_register.eint: %u", status_register.eint);
    NRF_LOG_INFO("status_register.eovf: %u", status_register.eovf);
    NRF_LOG_INFO("status_register.fstint: %u", status_register.fstint);
    NRF_LOG_INFO("status_register.dcloffint: %u", status_register.dcloffint);

    NRF_LOG_INFO("status_register.lonint: %u", status_register.lonint);
    NRF_LOG_INFO("status_register.rrint: %u", status_register.rrint);
    NRF_LOG_INFO("status_register.samp: %u", status_register.samp);
    NRF_LOG_INFO("status_register.pllint: %u", status_register.pllint);

    NRF_LOG_INFO("status_register.ldoff_ph: %u", status_register.ldoff_ph);
    NRF_LOG_INFO("status_register.ldoff_pl: %u", status_register.ldoff_pl);
    NRF_LOG_INFO("status_register.ldoff_nh: %u", status_register.ldoff_nh);
    NRF_LOG_INFO("status_register.ldoff_nl: %u", status_register.ldoff_nl);
}


/* 
* Read EN_INT1 (0x02) as an Interrupt Register to read the following values:
* D[23], EN_EINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[22], EN_EOVF = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[21], EN_FSTINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[20], EN_DCLOFFINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[19] - D[12], NOT USED 
* D[11], EN_LONINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[10], EN_RRINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[9], EN_SAMP = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[8], EN_PLLINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[7] - D[2], NOT USED
* D[1:0], INTB_TYPE1 = INT1B Port Type (EN_INT1 Selections)
*/
static void _max30003_read_interrupt1_register(void)
{
    _max30003_spim_read_registers(interrupt1_register.register_pointer, control.spi_data, control.register_byte_count);

    interrupt1_register.en_eint = (control.spi_data[0] & 0b10000000) && 0b10000000;
    interrupt1_register.en_eovf = (control.spi_data[0] & 0b01000000) && 0b01000000;
    interrupt1_register.en_fstint = (control.spi_data[0] & 0b00100000) && 0b00100000;
    interrupt1_register.en_dcloffint = (control.spi_data[0] & 0b00010000) && 0b00010000;

    interrupt1_register.en_lonint = (control.spi_data[1] & 0b00001000) && 0b00001000; 
    interrupt1_register.en_rrint = (control.spi_data[1] & 0b00000100) && 0b00000100;
    interrupt1_register.en_samp = (control.spi_data[1] & 0b00000010) && 0b00000010;
    interrupt1_register.en_pllint = (control.spi_data[1] & 0b00000001) && 0b00000001;

    interrupt1_register.int1b_port = control.spi_data[2] & 0b00000011;

    NRF_LOG_INFO("interrupt1_register.en_eint: %u", interrupt1_register.en_eint);
    NRF_LOG_INFO("interrupt1_register.en_eovf: %u", interrupt1_register.en_eovf);
    NRF_LOG_INFO("interrupt1_register.en_fstint: %u", interrupt1_register.en_fstint);
    NRF_LOG_INFO("interrupt1_register.en_dcloffint: %u", interrupt1_register.en_dcloffint);

    NRF_LOG_INFO("interrupt1_register.en_lonint: %u", interrupt1_register.en_lonint);
    NRF_LOG_INFO("interrupt1_register.en_rrint: %u", interrupt1_register.en_rrint);
    NRF_LOG_INFO("interrupt1_register.en_samp: %u", interrupt1_register.en_samp);
    NRF_LOG_INFO("interrupt1_register.en_pllint: %u", interrupt1_register.en_pllint);

    NRF_LOG_INFO("interrupt1_register.int1b_port: %u", interrupt1_register.int1b_port);
}

static void _max30003_write_interrupt1_register(void)
{
    NRF_LOG_INFO("max30003_write_interrupt1_register");
    control.spi_data[0] = interrupt1_register.en_eint << 7;
    control.spi_data[0] = (interrupt1_register.en_eovf << 6) | control.spi_data[0];
    control.spi_data[0] = (interrupt1_register.en_fstint << 5) | control.spi_data[0];
    control.spi_data[0] = (interrupt1_register.en_dcloffint << 4) | control.spi_data[0];

    control.spi_data[1] = interrupt1_register.en_lonint << 3;
    control.spi_data[1] = (interrupt1_register.en_rrint << 2) | control.spi_data[1];
    control.spi_data[1] = (interrupt1_register.en_samp << 1) | control.spi_data[1];
    control.spi_data[1] = interrupt1_register.en_pllint | control.spi_data[1];

    control.spi_data[2] = interrupt1_register.int1b_port;

    _max30003_spim_write_registers(interrupt1_register.register_pointer, control.spi_data, control.register_byte_count);
}

/*
* Read EN_INT2 (0x02) as an Interrupt Register to read the following values:
* D[23], EN_EINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[22], EN_EOVF = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[21], EN_FSTINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[20], EN_DCLOFFINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[19] - D[12], NOT USED 
* D[11], EN_LONINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[10], EN_RRINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[9], EN_SAMP = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[8], EN_PLLINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[7] - D[2], NOT USED
* D[1:0], INTB_TYPE0 = INT2B Port Type (EN_INT2 Selections)
*/
static void _max30003_read_interrupt2_register(void)
{
    NRF_LOG_INFO("max30003_read_interrupt2_register");
    _max30003_spim_read_registers(interrupt2_register.register_pointer, control.spi_data, control.register_byte_count);

    interrupt2_register.en_eint = (control.spi_data[0] & 0b10000000) && 0b10000000;
    interrupt2_register.en_eovf = (control.spi_data[0] & 0b01000000) && 0b01000000;
    interrupt2_register.en_fstint = (control.spi_data[0] & 0b00100000) && 0b00100000;
    interrupt2_register.en_dcloffint = (control.spi_data[0] & 0b00010000) && 0b00010000;

    interrupt2_register.en_lonint = (control.spi_data[1] & 0b00001000) && 0b00001000; 
    interrupt2_register.en_rrint = (control.spi_data[1] & 0b00000100) && 0b00000100;
    interrupt2_register.en_samp = (control.spi_data[1] & 0b00000010) && 0b00000010;
    interrupt2_register.en_pllint = (control.spi_data[1] & 0b00000001) && 0b00000001;

    interrupt2_register.int1b_port = control.spi_data[2] & 0b00000011;

    NRF_LOG_INFO("interrupt2_register.en_eint: %u", interrupt2_register.en_eint);
    NRF_LOG_INFO("interrupt2_register.en_eovf: %u", interrupt2_register.en_eovf);
    NRF_LOG_INFO("interrupt2_register.en_fstint: %u", interrupt2_register.en_fstint);
    NRF_LOG_INFO("interrupt2_register.en_dcloffint: %u", interrupt2_register.en_dcloffint);

    NRF_LOG_INFO("interrupt2_register.en_lonint: %u", interrupt2_register.en_lonint);
    NRF_LOG_INFO("interrupt2_register.en_rrint: %u", interrupt2_register.en_rrint);
    NRF_LOG_INFO("interrupt2_register.en_samp: %u", interrupt2_register.en_samp);
    NRF_LOG_INFO("interrupt2_register.en_pllint: %u", interrupt2_register.en_pllint);

    NRF_LOG_INFO("interrupt2_register.int1b_port: %u", interrupt2_register.int1b_port);
}

static void _max30003_write_interrupt2_register(void)
{
    NRF_LOG_INFO("max30003_write_interrupt2_register");
    control.spi_data[0] = interrupt2_register.en_eint << 7;
    control.spi_data[0] = (interrupt2_register.en_eovf << 6) | control.spi_data[0];
    control.spi_data[0] = (interrupt2_register.en_fstint << 5) | control.spi_data[0];
    control.spi_data[0] = (interrupt2_register.en_dcloffint << 4) | control.spi_data[0];

    control.spi_data[1] = interrupt2_register.en_lonint << 3;
    control.spi_data[1] = (interrupt2_register.en_rrint << 2) | control.spi_data[1];
    control.spi_data[1] = (interrupt2_register.en_samp << 1) | control.spi_data[1];
    control.spi_data[1] = interrupt2_register.en_pllint | control.spi_data[1];

    control.spi_data[2] = interrupt2_register.int1b_port;

    _max30003_spim_write_registers(interrupt2_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read Interrupt Manager Register to read the following values:
* D[23:19], EFIT = ECG FIFO Interrupt Threshold, Active High; Issues EINT based on number of unread FIFO records
* D[19:7], NOT USED
* D[6], CLR_FAST = Fast Mode interrupt clear behavoir; 1 = FSTINT remains active until cleared by STATUS read back
* D[5:4], CLR_RRINT = RTOS R Detect Interrupt (RRINT) Clear Behavoir; many options available, see datasheet
* D[3], NOT USED 
* D[2], CLR_SAMP = Sample Synchronization Pulse clear behavoir
* D[1:0], SAMP_IT = Sample Synchronization Pulse Frequency
*/
static void _max30003_read_interrupt_manager_register(void)
{
    NRF_LOG_INFO("max30003_read_interrupt_manager_register");
    _max30003_spim_read_registers(interrupt_manager_register.register_pointer, control.spi_data, control.register_byte_count);

    interrupt_manager_register.efit = (control.spi_data[0] & 0b11111000) >> 3;

    interrupt_manager_register.clr_fast = (control.spi_data[2] & 0b01000000) && 0b01000000;
    interrupt_manager_register.clr_rrint = (control.spi_data[2] & 0b00110000) >> 4;
    interrupt_manager_register.clr_samp = (control.spi_data[2] & 0b00000100) && 0b00000100;
    interrupt_manager_register.samp_it = control.spi_data[2] & 0b00000011; 

    NRF_LOG_INFO("interrupt_manager_register.efit: %u", interrupt_manager_register.efit);

    NRF_LOG_INFO("interrupt_manager_register.clr_fast: %u", interrupt_manager_register.clr_fast);
    NRF_LOG_INFO("interrupt_manager_register.clr_rrint: %u", interrupt_manager_register.clr_rrint);
    NRF_LOG_INFO("interrupt_manager_register.clr_samp: %u", interrupt_manager_register.clr_samp);
    NRF_LOG_INFO("interrupt_manager_register.samp_it: %u", interrupt_manager_register.samp_it);
}

static void _max30003_write_interrupt_manager_register(void)
{
    NRF_LOG_INFO("max30003_write_interrupt_manager_register");
    control.spi_data[0] = interrupt_manager_register.efit << 3;
    
    control.spi_data[1] = 0;

    control.spi_data[2] = interrupt_manager_register.clr_fast << 6;
    control.spi_data[2] = (interrupt_manager_register.clr_rrint << 4) | control.spi_data[2];
    control.spi_data[2] = (interrupt_manager_register.clr_samp << 2) | control.spi_data[2];
    control.spi_data[2] = interrupt_manager_register.samp_it | control.spi_data[2];

    _max30003_spim_write_registers(interrupt_manager_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read Dynamic Mode Manager Register to read the following values:
* D[23:22], FAST = ECG Channel Fast Recovery Mode Selection (ECG High Pass Filter Bypass)
* D[21:16], FAST_TH = Automatic Fast Recovery Threshold
* D[15:0], NOT USED 
*/
static void _max30003_read_dynamic_mode_manager_register(void)
{
    NRF_LOG_INFO("max30003_read_dynamic_mode_manager_register");
    _max30003_spim_read_registers(dynamic_mode_manager_register.register_pointer, control.spi_data, control.register_byte_count);

    dynamic_mode_manager_register.fast = (control.spi_data[0] & 0b11000000) >> 6;
    dynamic_mode_manager_register.fast_th = control.spi_data[0] & 0b00111111; 

    NRF_LOG_INFO("dynamic_mode_manager_register.fast: %u", dynamic_mode_manager_register.fast);
    NRF_LOG_INFO("dynamic_mode_manager_register.fast_th: %u", dynamic_mode_manager_register.fast_th);
}

static void _max30003_write_dynamic_mode_manager_register(void)
{
    NRF_LOG_INFO("max30003_write_dynamic_mode_manager_register");
    control.spi_data[0] = dynamic_mode_manager_register.fast << 6;
    control.spi_data[0] = dynamic_mode_manager_register.fast_th | control.spi_data[0];

    control.spi_data[1] = 0;
    control.spi_data[2] = 0;

    _max30003_spim_write_registers(dynamic_mode_manager_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Write Software Reset Register to write the following values:
* D[23:0], SW_RST = Software Reset; Resets the MAX30003 to its original default condition at the end of the SPI SW_RST transaction
* A Software Reset is identical to power-cycling the device
* Execution occurs only if DIN[23:0] = 0x000000
*/
static void _max30003_write_software_reset_register(void)
{
    NRF_LOG_INFO("max30003_write_software_reset_register");
    control.spi_data[0] = 0;
    control.spi_data[1] = 0;
    control.spi_data[2] = 0;
    _max30003_spim_write_registers(software_reset_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Write Synchronize Register to write the following values:
* D[23:0], SYNCH = Synchronize; Write-only register/command that begins new ECG operations and recording, beginning on the internal MSTR clock edge
* In addition to resetting and synchronizing the operations of any active ECG and R-R circuitry, SYNCH will reset and clear the FIFO memories and the DSP filters
* Execution occurs only if DIN[23:0] = 0x000000
* If a FIFO overflow event occurs and a portion of the record is lost, it is recommended to use the SYNCH command to recover and restart the recording
* 
*/
static void _max30003_write_synchronize_register(void)
{
    NRF_LOG_INFO("max30003_write_synchronize_register");
    control.spi_data[0] = 0;
    control.spi_data[1] = 0;
    control.spi_data[2] = 0;
    _max30003_spim_write_registers(synchronization_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Write FIFO Reset Register to write the following values:
* D[23:0], FIFO_RST = FIFO Reset; Write-only register/command that begins a new ECG recording by resetting the FIFO memories and resuming the record
* with the next available ECG data. Operations of any active ECG and R-R circuit are not impacted by FIFO_RST. 
* FIFO_RST can be used to quickly recover from a FIFO overflow state
* Execution occurs only if DIN[23:0] = 0x000000 
*/
static void _max30003_write_fifo_reset_register(void)
{
    NRF_LOG_INFO("max30003_write_fifo_reset_register");
    control.spi_data[0] = 0;
    control.spi_data[1] = 0;
    control.spi_data[2] = 0;
    _max30003_spim_write_registers(fifo_reset_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read Info Register to read the following values:
* D[23:20; 13:12] , INFO_ID = Interface Verification Code;
* D[19:16], REV_ID = Revision ID
* D[15:14; 11:0], NOT USED
*
* Due to internal initialization procedures, this command will not read-back valid data if it is the first command executed following either
* a power-cycle event, or a SW_RST event.
*/
static void _max30003_read_info_register(void)
{
    NRF_LOG_INFO("max30003_read_info_register");
    _max30003_spim_read_registers(info_register.register_pointer, control.spi_data, control.register_byte_count);

    info_register.info_id = ((control.spi_data[0] & 0b11110000) >> 2) | ((control.spi_data[1] & 0b00110000) >> 4);
    info_register.rev_id = control.spi_data[0] & 0b00001111;
     
    NRF_LOG_INFO("info_register.info_id: %u", info_register.info_id);
    NRF_LOG_INFO("info_register.rev_id: %u", info_register.rev_id);
}

/* 
* Readand Write General Configuration Register to read and write the following values:
* D[23:22], EN_ULP_LON = Ultra-Low Power Lead-On Detection Enable; ULP mode is only active when the ECG channel is powered down/disabled
* D[21:20], FMSTR = Master Clock Frequency; These are generated from FCLK, which is always 32.768 kHz
* D[19], EN_ECG = ECG Channel Enable; 1 = Enabled, 0 = Disabled; The ECG channel must be enabled to allow R-R operation 
* D[18:14], NOT USED
* D[13:12], EN_DCLOFF = DC Lead-Off Detection Enable; Requires active selected channel, enables DCLOFF interrupt and status bit behavoir 
* D[11], DCLOFF_IPOL = DC Lead-Off current polarity (if current sources are enabled/connected)
* D[10:8], DCLOFF_IMAG = DC Lead-Off Current Magnitude Selection
* D[7:6], DCLOFF_VTH = DC Lead-Off Voltage Threshold Selection
* D[5:4], EN_RBIAS = Enable and select Resistive Lead Bias Mode
* D[3:2], RBIAS = Resistive Bias Mode Value Selection
* D[1], RBIASP = Enables Resistive Bias on Positive Input
* D[0], RBIASN = Enables Resistive Bias on Negative Input
*/
static void _max30003_read_general_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_general_configuration_register");
    _max30003_spim_read_registers(general_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    general_configuration_register.en_ulp_lon = (control.spi_data[0] & 0b11000000) >> 6;
    general_configuration_register.fmstr = (control.spi_data[0] & 0b00110000) >> 4;
    general_configuration_register.en_ecg = (control.spi_data[0] & 0b00001000) && 0b00001000;
    general_configuration_register.en_dcloff = (control.spi_data[1] & 0b00110000) >> 4;
    general_configuration_register.dcloff_ipol = (control.spi_data[1] & 0b00001000) && 0b00001000;
    general_configuration_register.dcloff_imag = control.spi_data[1] & 0b00000111;
    general_configuration_register.dcloff_vth = (control.spi_data[2] & 0b11000000) >> 6;
    general_configuration_register.en_rbias = (control.spi_data[2] & 0b00110000) >> 4;
    general_configuration_register.rbiasv = (control.spi_data[2] & 0b00001100) >> 2;
    general_configuration_register.rbiasp = (control.spi_data[2] & 0b00000010) >> 1;
    general_configuration_register.rbiasn = control.spi_data[2] & 0b00000001;

    NRF_LOG_INFO("general_configuration_register.en_ulp_lon: %u", general_configuration_register.en_ulp_lon);
    NRF_LOG_INFO("general_configuration_register.fmstr: %u", general_configuration_register.fmstr);
    NRF_LOG_INFO("general_configuration_register.en_ecg: %u", general_configuration_register.en_ecg);
    NRF_LOG_INFO("general_configuration_register.en_dcloff: %u", general_configuration_register.en_dcloff);
    NRF_LOG_INFO("general_configuration_register.dcloff_ipol: %u", general_configuration_register.dcloff_ipol);
    NRF_LOG_INFO("general_configuration_register.dcloff_imag: %u", general_configuration_register.dcloff_imag);
    NRF_LOG_INFO("general_configuration_register.dcloff_vth: %u", general_configuration_register.dcloff_vth);
    NRF_LOG_INFO("general_configuration_register.en_rbias: %u", general_configuration_register.en_rbias);
    NRF_LOG_INFO("general_configuration_register.rbiasv: %u", general_configuration_register.rbiasv);
    NRF_LOG_INFO("general_configuration_register.rbiasp: %u", general_configuration_register.rbiasp);
    NRF_LOG_INFO("general_configuration_register.rbiasn: %u", general_configuration_register.rbiasn);
}

static void _max30003_write_general_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_general_configuration_register");
    control.spi_data[0] = general_configuration_register.en_ulp_lon << 6;
    control.spi_data[0] = (general_configuration_register.fmstr << 4) | control.spi_data[0];
    control.spi_data[0] = (general_configuration_register.en_ecg << 3) | control.spi_data[0];
    
    control.spi_data[1] = general_configuration_register.en_dcloff << 4;
    control.spi_data[1] = (general_configuration_register.dcloff_ipol << 3) | control.spi_data[1];
    control.spi_data[1] = general_configuration_register.dcloff_imag | control.spi_data[1];
    
    control.spi_data[2] = general_configuration_register.dcloff_vth << 6;
    control.spi_data[2] = (general_configuration_register.en_rbias << 4) | control.spi_data[2];
    control.spi_data[2] = (general_configuration_register.rbiasv << 2) | control.spi_data[2];
    control.spi_data[2] = (general_configuration_register.rbiasp << 1) | control.spi_data[2];
    control.spi_data[2] = general_configuration_register.rbiasn | control.spi_data[2];

    _max30003_spim_write_registers(general_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read and Write Calibration Configuration Register to read and write the following values:
* D[23], NOT USED
* D[22], EN_VCAL = Calibration Source (VCALP and VCALN) Enable
* D[21], VMODE = Calibration Source Mode Selection
* D[20], VMAG = Calibration Source Magnitude Selection (VMAG)
* D[19:15], NOT USED
* D[14:12], FCAL = Calibration Source Frequency Selection (FCAL)
* D[11], FIFTY = Calibration Soruce Duty Cycle Mode Selection
* D[10:0], THIGH = Calibration Source Time High Selection
*/
static void _max30003_read_calibration_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_calibration_configuration_register");
    _max30003_spim_read_registers(calibration_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    calibration_configuration_register.en_vcal = (control.spi_data[0] & 0b01000000) && 0b01000000;
    calibration_configuration_register.vmode = (control.spi_data[0] & 0b00100000) && 0b00100000;
    calibration_configuration_register.vmag = (control.spi_data[0] & 0b00010000) && 0b00010000;
    calibration_configuration_register.fcal = (control.spi_data[1] & 0b01110000) >> 4;
    calibration_configuration_register.fifty = (control.spi_data[1] & 0b00001000) && 0b00001000;
    calibration_configuration_register.thigh = ((control.spi_data[1] & 0b00000111) << 8) | (control.spi_data[2] & 0b11111111);

    NRF_LOG_INFO("calibration_configuration_register.en_vcal: %u", calibration_configuration_register.en_vcal);
    NRF_LOG_INFO("calibration_configuration_register.vmode: %u", calibration_configuration_register.vmode);
    NRF_LOG_INFO("calibration_configuration_register.vmag: %u", calibration_configuration_register.vmag);
    NRF_LOG_INFO("calibration_configuration_register.fcal: %u", calibration_configuration_register.fcal);
    NRF_LOG_INFO("calibration_configuration_register.fifty: %u", calibration_configuration_register.fifty);
    NRF_LOG_INFO("calibration_configuration_register.thigh: %u", calibration_configuration_register.thigh);
}

static void _max30003_write_calibration_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_calibration_configuration_register");
    control.spi_data[0] = calibration_configuration_register.en_vcal << 6;
    control.spi_data[0] = (calibration_configuration_register.vmode << 5) | control.spi_data[0];
    control.spi_data[0] = (calibration_configuration_register.vmag << 4) | control.spi_data[0];
    
    control.spi_data[1] = calibration_configuration_register.fcal << 4;
    control.spi_data[1] = (calibration_configuration_register.fifty << 3) | control.spi_data[1];
    control.spi_data[1] = (uint8_t)((calibration_configuration_register.thigh & 0b0000011100000000) >> 8 )  | control.spi_data[1];
    
    control.spi_data[2] = (uint8_t)(calibration_configuration_register.thigh & 0b0000000011111111);

    _max30003_spim_write_registers(calibration_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read and Write Input Multiplexer Configuration Register to read and write the following values:
* D[23], POL = ECG Input Polarity Selection
* D[22], NOT USED
* D[21], OPENP = Open the ECGP Input Switch
* D[20], OPENN = Open the ECGN Input Switch
* D[19:18], CALP_SEL = ECGP Calibration Selection
* D[17:16], CALN_SEL = ECGN Calibration Selection
* D[15:0], NOT USED
*/
static void _max30003_read_input_multiplexer_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_input_multiplexer_configuration_register");
    _max30003_spim_read_registers(input_multiplexer_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    input_multiplexer_configuration_register.pol = (control.spi_data[0] & 0b10000000) && 0b10000000;
    input_multiplexer_configuration_register.openp = (control.spi_data[0] & 0b00100000) && 0b00100000;
    input_multiplexer_configuration_register.openn = (control.spi_data[0] & 0b00010000) && 0b00010000;
    input_multiplexer_configuration_register.calp_sel = (control.spi_data[0] & 0b00001100) >> 2;
    input_multiplexer_configuration_register.caln_sel = control.spi_data[0] & 0b00000011;

    NRF_LOG_INFO("input_multiplexer_configuration_register.pol: %u", input_multiplexer_configuration_register.pol);
    NRF_LOG_INFO("input_multiplexer_configuration_register.openp: %u", input_multiplexer_configuration_register.openp);
    NRF_LOG_INFO("input_multiplexer_configuration_register.openn: %u", input_multiplexer_configuration_register.openn);
    NRF_LOG_INFO("input_multiplexer_configuration_register.calp_sel: %u", input_multiplexer_configuration_register.calp_sel);
    NRF_LOG_INFO("input_multiplexer_configuration_register.caln_sel: %u", input_multiplexer_configuration_register.caln_sel);
}

static void _max30003_write_input_multiplexer_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_input_multiplexer_configuration_register");
    control.spi_data[0] = input_multiplexer_configuration_register.pol << 7;
    control.spi_data[0] = (input_multiplexer_configuration_register.openp << 5) | control.spi_data[0];
    control.spi_data[0] = (input_multiplexer_configuration_register.openn << 4) | control.spi_data[0];
    control.spi_data[0] = (input_multiplexer_configuration_register.calp_sel << 2) | control.spi_data[0];
    control.spi_data[0] = input_multiplexer_configuration_register.caln_sel | control.spi_data[0];
    
    control.spi_data[1] = 0;
    control.spi_data[2] = 0;

    _max30003_spim_write_registers(input_multiplexer_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read and Write ECG Configuration Register to read and write the following values:
* D[23:22], RATE = ECG Data Rate Dependent on the F_MSTR
* D[21:18], NOT USED
* D[17:16], GAIN = ECG Channel Gain Setting
* D[15], NOT USED
* D[14], DHPF = ECG Channel Digital High-Pass Filter Cutoff Frequency
* D[13:12], DLPF = ECGP Channel Digital Low-Pass Filter Cutoff Frequency
* D[11:0], NOT USED
*/
static void _max30003_read_ecg_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_ecg_configuration_register");
    _max30003_spim_read_registers(ecg_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    ecg_configuration_register.rate = (control.spi_data[0] & 0b11000000) >> 6;
    ecg_configuration_register.gain = control.spi_data[0] & 0b00000011;

    ecg_configuration_register.dhpf = (control.spi_data[1] & 0b01000000) && 0b01000000;
    ecg_configuration_register.dlpf = (control.spi_data[1] & 0b00110000) >> 4;

    NRF_LOG_INFO("ecg_configuration_register.rate: %u", ecg_configuration_register.rate);
    NRF_LOG_INFO("ecg_configuration_register.gain: %u", ecg_configuration_register.gain);
    NRF_LOG_INFO("ecg_configuration_register.dhpf: %u", ecg_configuration_register.dhpf);
    NRF_LOG_INFO("ecg_configuration_register.dlpf: %u", ecg_configuration_register.dlpf);
}

static void _max30003_write_ecg_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_ecg_configuration_register");
    control.spi_data[0] = ecg_configuration_register.rate << 6;
    control.spi_data[0] = ecg_configuration_register.gain | control.spi_data[0];
    control.spi_data[1] = (ecg_configuration_register.dhpf << 6) | control.spi_data[1];
    control.spi_data[1] = (ecg_configuration_register.dlpf << 4) | control.spi_data[1];

    control.spi_data[2] = 0;
    
    _max30003_spim_write_registers(ecg_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read and Write R-R #1 Configuration Register to read and write the following values:
* D[23:20], WNDW = This is the Width of the Averaging Window; Adjusts the Algorithm Sensitivity to the Width of the QRS Complex
* D[29:16], GAIN = R to R Gain; This is Used to Maximize the Dynamic Range of the Algorithm
* D[15], EN_RTOR = ECG RTOR Detection Enable
* D[14], NOT USED
* D[13:12], PACG = R to R Peak Averaging Weight Factor
* D[11:8], PTSF = R to R Peak Threshold Scaling Factor
* D[7:0], NOT USED
*/
static void _max30003_read_rtor1_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_rtor1_configuration_register");
    _max30003_spim_read_registers(rtor1_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    rtor1_configuration_register.wndw = (control.spi_data[0] & 0b11110000) >> 4;
    rtor1_configuration_register.gain = control.spi_data[0] & 0b00001111;

    rtor1_configuration_register.en_rtor = (control.spi_data[1] & 0b10000000) && 0b10000000;
    rtor1_configuration_register.pavg = (control.spi_data[1] & 0b00110000) >> 4;
    rtor1_configuration_register.ptsf = control.spi_data[1] & 0b00001111;

    NRF_LOG_INFO("rtor1_configuration_register.wndw: %u", rtor1_configuration_register.wndw);
    NRF_LOG_INFO("rtor1_configuration_register.gain: %u", rtor1_configuration_register.gain);
    NRF_LOG_INFO("rtor1_configuration_register.en_rtor: %u", rtor1_configuration_register.en_rtor);
    NRF_LOG_INFO("rtor1_configuration_register.pavg: %u", rtor1_configuration_register.pavg);
    NRF_LOG_INFO("rtor1_configuration_register.ptsf: %u", rtor1_configuration_register.ptsf);
}

static void _max30003_write_rtor1_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_rtor1_configuration_register");
    control.spi_data[0] = rtor1_configuration_register.wndw << 4;
    control.spi_data[0] = rtor1_configuration_register.gain | control.spi_data[0];
    control.spi_data[1] = (rtor1_configuration_register.en_rtor << 7) | control.spi_data[1];
    control.spi_data[1] = (rtor1_configuration_register.pavg << 4) | control.spi_data[1];
    control.spi_data[1] = rtor1_configuration_register.ptsf | control.spi_data[1];
    
    control.spi_data[2];

    _max30003_spim_write_registers(rtor1_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

/* 
* Read and Write R-R #2 Configuration Register to read and write the following values:
* D[23:22], NOT USED
* D[21:16], HOFF = R to R Minimum Hold Off; Sets the Absolute Minimum Interval Used for the Static Portion of the Hold Off Criteria
* D[15:14], NOT USED
* D[13:12], GAIN = R to R Interval Averaging Weight Factor; Weighting Factor for the Current R to R interval observation
* D[11], NOT USED
* D[10:8], RHSF = R to R Interval Hold Off Scaling Factor; Fraction of the R to R Average Interval Used for the Dynamic Portion of the Holdoff Criteria
* D[7:0], NOT USED
*/
static void _max30003_read_rtor2_configuration_register(void)
{
    NRF_LOG_INFO("max30003_read_rtor2_configuration_register");
    _max30003_spim_read_registers(rtor2_configuration_register.register_pointer, control.spi_data, control.register_byte_count);

    rtor2_configuration_register.hoff = control.spi_data[0] & 0b00111111;

    rtor2_configuration_register.ravg = (control.spi_data[1] & 0b00110000) >> 4;
    rtor2_configuration_register.rhsf = control.spi_data[1] & 0b00000111;

    NRF_LOG_INFO("rtor2_configuration_register.hoff: %u", rtor2_configuration_register.hoff);
    NRF_LOG_INFO("rtor2_configuration_register.ravg: %u", rtor2_configuration_register.ravg);
    NRF_LOG_INFO("rtor2_configuration_register.rhsf: %u", rtor2_configuration_register.rhsf);
}

static void _max30003_write_rtor2_configuration_register(void)
{
    NRF_LOG_INFO("max30003_write_rtor2_configuration_register");
    control.spi_data[0] = rtor2_configuration_register.hoff;
    control.spi_data[1] = rtor2_configuration_register.ravg << 4;
    control.spi_data[1] = rtor2_configuration_register.rhsf | control.spi_data[1];
    
    control.spi_data[2] = 0;

    _max30003_spim_write_registers(rtor2_configuration_register.register_pointer, control.spi_data, control.register_byte_count);
}

static void _max30003_spim_read_registers(uint8_t register_address, uint8_t *data, uint8_t data_length)
{
    NRF_LOG_INFO("_max30003_spim_read_registers");
   
    uint8_t temp_data_length = data_length + 1;
    uint8_t temp_data[temp_data_length];
    NRF_LOG_INFO("temp_data_length: %u", temp_data_length);
    spim_read_registers(register_address, temp_data, temp_data_length);
    
    temp_data[0] = register_address;
    for(uint8_t i = 1; i < temp_data_length; i++)
    {
        data[i-1] = temp_data[i];
    }
    if(temp_data_length == 0x04)
    {
        NRF_LOG_INFO("Register Address: %X; Array Value: %X %X %X", temp_data[0], temp_data[1], temp_data[2], temp_data[3]); // Debugging purposes only
    }
}

static void _max30003_spim_write_registers(uint8_t start_register_address, uint8_t *data, uint8_t data_length)
{
    NRF_LOG_INFO("_max30003_spim_write_registers");
    uint8_t temp_data[data_length + 1];     // Debugging purposes only
    temp_data[0] = start_register_address;    // Debugging purposes only
    for(uint8_t i = 1; i <= data_length; i++)
    {
        temp_data[i] = data[i-1];       // Debugging purposes only
    }
    NRF_LOG_HEXDUMP_INFO(temp_data, ARRAY_LENGTH(temp_data));   // Debugging purposes only
    spim_write_registers(start_register_address, data, data_length);
}
#endif