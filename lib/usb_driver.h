/* 
 * File:   usb_driver.h
 * Author: mplab
 *
 * Created on April 14, 2026, 12:58 AM
 */

#ifndef USB_DRIVER_H
#define	USB_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>

#define USB_REPORT_PERIOD_MS 512
#define USB_TYPE_MOUSE 0
#define USB_TYPE_KEYBOARD_PRESS 1
#define USB_TYPE_KEYBOARD_RELEASE 2

void Initialize_USB(void);

int set_next_keyboard_press(bool overwrite, uint8_t* report);

int delta_next_mouse_move(int dx_change, int dy_change);

void Process_USB_Reports(void);


#ifdef	__cplusplus
}
#endif

#endif	/* USB_DRIVER_H */

