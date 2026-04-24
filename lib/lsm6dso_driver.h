/* 
 * File:   lsm6dso_driver.h
 * Author: mplab
 *
 * Created on April 15, 2026, 5:50 PM
 */

#ifndef LSM6DSO_DRIVER_H
#define	LSM6DSO_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LSM6DSO_ADDR 0x6B
#define LSM6DSO_WHO_AM_I_REG 0x0F
#define LSM6DSO_CTRL1_XL     0x10
#define LSM6DSO_WAKE_UP_SRC  0x1B
#define LSM6DSO_TAP_CFG2     0x58
#define LSM6DSO_WAKE_UP_THS  0x5B
#define LSM6DSO_WAKE_UP_DUR  0x5C
    
void Initialize_IMU();    

bool detect_shake_event(uint8_t shaking_ind_bm);


#ifdef	__cplusplus
}
#endif

#endif	/* LSM6DSO_DRIVER_H */

