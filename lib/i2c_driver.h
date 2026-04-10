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

#include <stdbool.h>

typedef struct {
    volatile uint8_t stage;
    uint8_t device_address;
    uint8_t rw_mode;
    uint8_t reg;
    uint8_t tx_data;
    volatile uint8_t rx_data;
} I2CCommand;

#define I2C_STATUS_UNKNOWN 0
#define I2C_STATUS_READY 1
#define I2C_STATUS_ADDRESSING 2
#define I2C_STATUS_ASUCCESS 3
#define I2C_STATUS_WRITING 4
#define I2C_STATUS_WSUCCESS 5
#define I2C_STATUS_RREADY 6
#define I2C_STATUS_RSUCCESS 7
#define I2C_STATUS_ERROR 8

#define CMD_STAGE_ADDR 0
#define CMD_STAGE_REG 1
#define CMD_STAGE_DATA 2
#define CMD_STAGE_REP 3
#define CMD_STAGE_DONE 4
#define CMD_STAGE_ERROR 5

extern const I2CCommand I2C_WRITE_CMD;
extern const I2CCommand I2C_READ_CMD;

extern volatile uint8_t status;

void InitializeTWI0();

int TWI0_WriteFull(uint8_t addr, uint8_t reg, uint8_t data);

int TWI0_ReadFull(uint8_t addr, uint8_t reg, uint8_t* data);

bool TWI0_IsBusy();

int TWI0_ReadAsync(I2CCommand* cmd);

int TWI0_WriteAsync(I2CCommand* cmd);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_DRIVER_H */

