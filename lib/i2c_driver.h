/* 
 * File:   i2c_driver.h
 * Author: mplab
 *
 * Created on April 9, 2026, 2:14 AM
 */

#pragma once
#ifndef I2C_DRIVER_H
#define	I2C_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define I2C_STATUS_UNKNOWN 0
#define I2C_STATUS_READY 1
#define I2C_STATUS_ADDRESSING 2
#define I2C_STATUS_ASUCCESS 3
#define I2C_STATUS_WRITING 4
#define I2C_STATUS_WSUCCESS 5
#define I2C_STATUS_RREADY 6
#define I2C_STATUS_RSUCCESS 7
#define I2C_STATUS_ERROR 8
    
void InitializeTWI0();

int TWI0_WriteFull(uint8_t addr, uint8_t reg, uint8_t data);

int TWI0_ReadFull(uint8_t addr, uint8_t reg, uint8_t* data);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_DRIVER_H */

