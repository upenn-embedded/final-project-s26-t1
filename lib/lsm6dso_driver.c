#include "i2c_driver.h"
#include "lsm6dso_driver.h"
#include <stdio.h>
#include <avr/io.h>

// target score required to trigger valid shake event
#define SHAKE_SCORE_THRESHOLD 2000

// points added to score when IMU registers movement
#define SCORE_ACTIVE_BUMP 2

// points subtracted from score when IMU registers no movement
#define SCORE_IDLE_DECAY 1

void Initialize_IMU() {
    InitializeTWI0();
    
    // rate is 416Hz, full scale is 2g
    I2CCommand ctrl1_cmd = I2C_WRITE_CMD;
    ctrl1_cmd.device_address = LSM6DSO_ADDR;
    ctrl1_cmd.reg = LSM6DSO_CTRL1_XL;
    ctrl1_cmd.tx_data = 0x50;
    
    TWI0_WriteAsync(&ctrl1_cmd);
        
    while (TWI0_IsBusy());
    
    // enable tap interrupts
    I2CCommand tap_cmd = I2C_WRITE_CMD;
    tap_cmd.device_address = LSM6DSO_ADDR;
    tap_cmd.reg = LSM6DSO_TAP_CFG2;
    tap_cmd.tx_data = 0x80;
    
    TWI0_WriteAsync(&tap_cmd);
    
    while (TWI0_IsBusy());
    
    // set wakeup threshold
    I2CCommand ths_cmd = I2C_WRITE_CMD;
    ths_cmd.device_address = LSM6DSO_ADDR;
    ths_cmd.reg = LSM6DSO_WAKE_UP_THS;
    ths_cmd.tx_data = 0x01;
    
    TWI0_WriteAsync(&ths_cmd);
        
    while (TWI0_IsBusy());
    
    // set wakeup duration to 0
    I2CCommand dur_cmd = I2C_WRITE_CMD;
    dur_cmd.device_address = LSM6DSO_ADDR;
    dur_cmd.reg = LSM6DSO_WAKE_UP_DUR;
    dur_cmd.tx_data = 0x00;
    
    TWI0_WriteAsync(&dur_cmd);
        
    while (TWI0_IsBusy());
    printf("Finished initializing IMU\n");
}

bool detect_shake_event(uint8_t shaking_ind_bm) {
    static int activity_score = 0;
    
    I2CCommand read_cmd = I2C_READ_CMD;
    read_cmd.device_address = LSM6DSO_ADDR;
    read_cmd.reg = LSM6DSO_WAKE_UP_SRC;

    TWI0_ReadAsync(&read_cmd);

    while (TWI0_IsBusy());
    
    if (read_cmd.stage == CMD_STAGE_DONE) {
        // bit 3 of wake_up_src is interrupt status
        if (read_cmd.rx_data & 0x08) {
            
            PORTD.OUTSET = shaking_ind_bm;
            
            // acceleration is above threshold so bump score
            activity_score += SCORE_ACTIVE_BUMP;
            
            if (activity_score >= SHAKE_SCORE_THRESHOLD) {
                // Shake complete
                activity_score = 0; 
                PORTD.OUTCLR = shaking_ind_bm;
                return true;
            }
            
        } else {
            // acceleration is below threshold so decrease score
            PORTD.OUTCLR = shaking_ind_bm;
            
            activity_score -= SCORE_IDLE_DECAY;
            
            if (activity_score < 0) {
                activity_score = 0;
            }
        }
    } else {
        printf("I2C Error (%d) reading WAKE_UP_SRC\n", read_cmd.stage);
    }
    
    return false;
}