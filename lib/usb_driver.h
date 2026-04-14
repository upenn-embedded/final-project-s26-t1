/* 
 * File:   usb_driver.h
 * Author: mplab
 *
 * Created on April 14, 2026, 12:58 AM
 */

#pragma once
#ifndef USB_DRIVER_H
#define	USB_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>

#define USB_REPORT_PERIOD_MS 512
#define USB_TYPE_MOUSE 0
#define USB_TYPE_KEYBOARD 1
    
#define LEFT_CLICK_PERIOD_MS 50
#define KBD_PRESS_PERIOD_MS 50

#define USB_LEFT_BTN 0x01
#define USB_NO_BTNS 0U
    
extern const uint8_t usb_kbd_super_k[9];

void Initialize_USB(void);

int set_keyboard_press(uint8_t* report);

int delta_next_mouse_move(int dx_change, int dy_change);

int set_left_click(bool pressed);

void Process_USB_Reports(void);


#ifdef	__cplusplus
}
#endif

#endif	/* USB_DRIVER_H */

