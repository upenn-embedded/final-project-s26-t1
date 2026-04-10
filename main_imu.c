#define F_CPU 4000000UL

#include "mcc_generated_files/system/system.h"
#include <util/delay.h>
#include "lib/i2c_driver.h"

#define LSM6DSO_ADDR 0x6B
#define LSM6DSO_WHO_AM_I_REG 0x0F

int main(void) {
    // initialize UART
    SYSTEM_Initialize();
    
    InitializeTWI0();
    
    while(1) {
        I2CCommand read_cmd = I2C_READ_CMD;
        read_cmd.device_address = LSM6DSO_ADDR;
        read_cmd.reg = LSM6DSO_WHO_AM_I_REG;
        
        printf("Starting I2C Transaction...\n");
        TWI0_ReadAsync(&read_cmd);
        
        while (TWI0_IsBusy());
        
        if (read_cmd.stage == CMD_STAGE_DONE) {
            printf("Data Received: 0x%02X\n", read_cmd.rx_data);
        } else {
            printf("I2C Error (%d), trying again...\n", read_cmd.stage);
        }
        
        _delay_ms(1000);
    }
    
    return 0;
}