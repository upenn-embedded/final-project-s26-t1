#include "../mcc_generated_files/usb/usb_device.h"
#include "../mcc_generated_files/usb/usb_common/usb_core_transfer.h"
#include "../mcc_generated_files/usb/usb_hid/usb_hid_transfer.h"
#include "../mcc_generated_files/usb/usb_peripheral/usb_peripheral_read_write.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "usb_driver.h"

const uint8_t usb_kbd_super_k[9] = {0x01, 0x08, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t usb_kbd_no_keys[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Report buffers
static uint8_t mouse_move_buffer[4]  = {0x02, 0x00, 0x00, 0x00};

volatile uint8_t mouse_buttons = 0;
volatile int mouse_delta[2] = {0, 0}; // (dx, dy)
volatile uint8_t* kbd_report = NULL;

volatile uint8_t next_report_type = USB_TYPE_MOUSE;

USB_PIPE_t hidPipe = {
    .address = 1,              
    .direction = USB_EP_DIR_IN 
};

void Initialize_USB() {
    // Ensure interrupts are disabled before starting
    cli();
    
    // Initialize MCC USB device
    USBDevice_Initialize();
    USB_Start();
    
    // Enable interrupts
    sei();
}

int set_keyboard_press(uint8_t* report) {
    kbd_report = report;
    return 0;
}

int delta_next_mouse_move(int dx_change, int dy_change) {
    mouse_delta[0] += dx_change;
    mouse_delta[1] += dy_change;
    return 0;
}

int set_left_click(bool pressed) {
    mouse_buttons = (pressed) ? USB_LEFT_BTN : USB_NO_BTNS;
    return 0;
}

int8_t clamp(int input) {
    return (input > 127) ? 127 : ((input < -127) ? -127 : input);
}

void Process_USB_Reports() {
    RETURN_CODE_t usb_status = USBDevice_Handle();
    
    int8_t local_dx, local_dy;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        local_dx = clamp(mouse_delta[0]);
        local_dy = clamp(mouse_delta[1]);
    }
    printf("%d | %d, %d\n", usb_status, mouse_delta[0], mouse_delta[1]);
    
    if (usb_status == SUCCESS) {
        if ((next_report_type == USB_TYPE_MOUSE || kbd_report == NULL) && (mouse_delta[0] != 0 || mouse_delta[1] != 0)) {
            mouse_move_buffer[1] = mouse_buttons;
            mouse_move_buffer[2] = local_dx;
            mouse_move_buffer[3] = local_dy;
            USB_TransferWriteStart(hidPipe, mouse_move_buffer, 4, false, NULL);
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                mouse_delta[0] -= local_dx;
                mouse_delta[1] -= local_dy;
            }
            next_report_type = USB_TYPE_KEYBOARD;
        } else if (next_report_type == USB_TYPE_KEYBOARD && kbd_report != NULL) {
            USB_TransferWriteStart(hidPipe, kbd_report, 9, false, NULL);
            next_report_type = USB_TYPE_MOUSE;
        } else if (next_report_type == USB_TYPE_KEYBOARD) {
            USB_TransferWriteStart(hidPipe, usb_kbd_no_keys, 9, false, NULL);
            next_report_type = USB_TYPE_MOUSE;
        }
     }
}