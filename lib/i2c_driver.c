#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "i2c_driver.h"

volatile uint8_t status = I2C_STATUS_UNKNOWN;

volatile I2CCommand* curr_cmd = NULL;

const I2CCommand I2C_WRITE_CMD = {
    .stage = CMD_STAGE_ADDR,
    .device_address = 0,
    .rw_mode = 0,
    .reg = 0,
    .tx_data = 0,
    .rx_data = 0
};

const I2CCommand I2C_READ_CMD = {
    .stage = CMD_STAGE_ADDR,
    .device_address = 0,
    .rw_mode = 1,
    .reg = 0,
    .tx_data = 0,
    .rx_data = 0
};

void InitializeTWI0() {
    status = I2C_STATUS_UNKNOWN;
    cli();
    
    // set pull ups since I2C is open drain
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm; 
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
    
    // f_SCL = 100kHZ (I2C standard mode)
    // t_R <= 1000ns
    // BAUD = f_CLK/(2*f_SCL) - (5 + (f_CLK*t_R)/2) = 13
    // t_OF <= 250ns
    // t_Low = (BAUD + 5)/f_CLK - t_OF = 4250ns
    // t_Lowsm = 4700ns > 4250ns (so we must recalculate BAUD)
    // BAUD_ACTUAL = f_CLK * (4700ns + 250 ns) - 3 = 16.58 ~= 16
    TWI0.MBAUD = 16;
    // enable TWI Host, write interrupt, and read interrupts
    TWI0.MCTRLA |= TWI_ENABLE_bm | TWI_WIEN_bm | TWI_RIEN_bm | TWI_SMEN_bm;
    // set MSTATUS to idle (starts out as unknown)
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
    
    sei();
    status = I2C_STATUS_READY;
}

// rw_bit = 1 -> read
// rw_bit = 0 -> write
void TWI0_SendAddr(uint8_t addr, uint8_t rw_bit) {
    status = I2C_STATUS_ADDRESSING;
    
    TWI0.MADDR = (addr << 1) | (rw_bit & 1);
}

uint8_t TWI0_Read(bool last_read) {
    status = I2C_STATUS_RSUCCESS;
    
    if (!last_read) {
        TWI0.MCTRLB &= ~(TWI_ACKACT_bm); // ACK
    } else {
        TWI0.MCTRLB |= TWI_ACKACT_bm; // NACK
    }
    
    uint8_t data = TWI0.MDATA;
    
    return data;
}

void TWI0_Write(uint8_t data) {
    status = I2C_STATUS_WRITING;
    
    TWI0.MDATA = data;
}

void TWI0_Stop() {
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
    
    status = I2C_STATUS_READY;
}

int TWI0_WriteFull(uint8_t addr, uint8_t reg, uint8_t data) {
    if (status == I2C_STATUS_UNKNOWN) {
        return -1;
    }
    
    TWI0_SendAddr(addr, 0);
    while (status != I2C_STATUS_ASUCCESS) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    
    TWI0_Write(reg);
    while (status != I2C_STATUS_WSUCCESS) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    
    TWI0_Write(data);
    while (status != I2C_STATUS_WSUCCESS) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    
    TWI0_Stop();
    
    return (status == I2C_STATUS_READY) ? 0 : -1;
}

int TWI0_ReadFull(uint8_t addr, uint8_t reg, uint8_t* data) {
    if (status == I2C_STATUS_UNKNOWN) {
        return -1;
    }
    
    TWI0_SendAddr(addr, 0);
    while (status != I2C_STATUS_ASUCCESS) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    
    TWI0_Write(reg);
    while (status != I2C_STATUS_WSUCCESS) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    
    TWI0_SendAddr(addr, 1);
    
    while (status != I2C_STATUS_RREADY) {
        if (status == I2C_STATUS_ERROR) return -1;
    }
    *data = TWI0_Read(true);
    
    TWI0_Stop();
    
    return (status == I2C_STATUS_READY) ? 0 : -1;
}

bool TWI0_IsBusy() {
    return curr_cmd != NULL && curr_cmd->stage != CMD_STAGE_ERROR &&
            curr_cmd->stage != CMD_STAGE_DONE;
}

int TWI0_ReadAsync(I2CCommand* cmd) {
    if (TWI0_IsBusy() || cmd == NULL) {
        return -1;
    }
    curr_cmd = cmd;
    
    TWI0_SendAddr(cmd->device_address, 0); // Reads always start as writes
    return 0;
}

int TWI0_WriteAsync(I2CCommand* cmd) {
    if (TWI0_IsBusy() || cmd == NULL) {
        return -1;
    }
    curr_cmd = cmd;
    
    TWI0_SendAddr(cmd->device_address, 0);
    return 0;
}

ISR(TWI0_TWIM_vect) {
    // check for errors (NACK received, bus error)
    if (TWI0.MSTATUS & (TWI_RXACK_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)) {
        status = I2C_STATUS_ERROR;
        TWI0_Stop();
        if (curr_cmd != NULL) {
            curr_cmd->stage = CMD_STAGE_ERROR;
            curr_cmd = NULL;
        }
        return;
    }
    
    // write interrupt
    if (TWI0.MSTATUS & TWI_WIF_bm) {
        if (status == I2C_STATUS_ADDRESSING) {
            status = I2C_STATUS_ASUCCESS;
            if (curr_cmd != NULL) {
                if (curr_cmd->stage == CMD_STAGE_ADDR) {
                    curr_cmd->stage = CMD_STAGE_REG;
                    TWI0_Write(curr_cmd->reg);
                } else if (curr_cmd->stage == CMD_STAGE_REP) {
                    curr_cmd->stage = CMD_STAGE_DATA;
                } else { // Unknown state
                    curr_cmd->stage = CMD_STAGE_ERROR;
                }
            }
        } else if (status == I2C_STATUS_WRITING) {
            status = I2C_STATUS_WSUCCESS;
            if (curr_cmd != NULL) {
                if (curr_cmd->stage == CMD_STAGE_REG && curr_cmd->rw_mode == 0) {
                    curr_cmd->stage = CMD_STAGE_DATA;
                    TWI0_Write(curr_cmd->tx_data);
                } else if (curr_cmd->stage == CMD_STAGE_REG && curr_cmd->rw_mode == 1) {
                    curr_cmd->stage = CMD_STAGE_REP;
                    TWI0_SendAddr(curr_cmd->device_address, 1);
                } else if (curr_cmd->stage == CMD_STAGE_DATA && curr_cmd->rw_mode == 0) {
                    TWI0_Stop();
                    curr_cmd->stage = CMD_STAGE_DONE;
                    curr_cmd = NULL;
                } else { // Unknown state
                    curr_cmd->stage = CMD_STAGE_ERROR;
                }
            }
        }
    }
    
    // read interrupt
    if (TWI0.MSTATUS & TWI_RIF_bm) {
        status = I2C_STATUS_RREADY;
        if (curr_cmd != NULL) {
            if ((curr_cmd->stage == CMD_STAGE_REP || 
                    curr_cmd->stage == CMD_STAGE_DATA) &&
                    curr_cmd->rw_mode == 1) {
                curr_cmd->rx_data = TWI0_Read(true);
                TWI0_Stop();
                curr_cmd->stage = CMD_STAGE_DONE;
                curr_cmd = NULL;
            }
        }
    }
}
