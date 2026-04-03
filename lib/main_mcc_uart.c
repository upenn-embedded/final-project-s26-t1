//#include "mcc_generated_files/system/system.h"
//#include <stdio.h>
//#include <util/delay.h>
//
//int main(void)
//{
//    // Initializes the clock, pins, and USART1
//    SYSTEM_Initialize();
//
//    while (1) {
//        printf("test\n");
//        
//        // Wait a bit so you don't flood the terminal
//        _delay_ms(20);
//    }
//}

#include "mcc_generated_files/system/system.h"
#include <stdio.h>
#include <util/delay.h>

#define LSM6DSO_ADDR         0x6B  // 7-bit I2C Address
#define LSM6DSO_WHO_AM_I_REG 0x0F  // Register address to read

int main(void)
{
    SYSTEM_Initialize();

    uint8_t reg_addr = LSM6DSO_WHO_AM_I_REG;
    uint8_t read_buffer = 0;

    printf("Starting I2C Test...\r\n");

    while (1) 
    {
        // 1. Initiate a Write-then-Read operation
        // This sends: [Start] [Addr+W] [Reg] [Restart] [Addr+R] [Data] [Stop]
        if (TWI0_WriteRead(LSM6DSO_ADDR, &reg_addr, 1, &read_buffer, 1))
        {
            // 2. Wait for the asynchronous transfer to complete
            while (TWI0_IsBusy());

            // 3. Check the result
            if (TWI0_ErrorGet() == I2C_ERROR_NONE)
            {
                printf("LSM6DSO WHO_AM_I: 0x%02X (Expected: 0x6C)\r\n", read_buffer);
            }
            else
            {
                printf("I2C Error Detected: %d\r\n", TWI0_ErrorGet());
            }
        }
        else
        {
            printf("TWI0 Bus was busy/not available\r\n");
        }

        _delay_ms(1000);
    }
}