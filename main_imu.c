#define F_CPU 4000000UL

#include "mcc_generated_files/system/system.h"
#include <util/delay.h>
#include "lib/i2c_driver.h"

#define LSM6DSO_ADDR 0x6B
#define LSM6DSO_WHO_AM_I_REG 0x0F

int main(void)
{
    // initialize UART
    SYSTEM_Initialize();
    
    InitializeTWI0();
    
    uint8_t data;
    int code;
    while(1) {
        printf("Starting I2C Transaction...")
        code = TWI0_ReadFull(LSM6DSO_ADDR, LSM6DSO_WHO_AM_I_REG, &data);
        if (code == 0) {
            printf("Data Received: 0x%02X\n", data);
        } else {
            printf("I2C Error (%d), trying again...\n", code);
        }
        
        _delay_ms(1000);
    }
    
    return 0;
}