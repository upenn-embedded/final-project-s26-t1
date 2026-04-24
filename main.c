#include "lib/usb_driver.h"
#include "mcc_generated_files/system/system.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lib/lsm6dso_driver.h"
#include <stdint.h>
#include <util/atomic.h>

// Port F buttons
#define LCLICK_BM PIN3_bm
#define VIRTKBD_BM PIN4_bm
#define QUIT_BM PIN5_bm
#define LCLICKHOLD_BM PIN6_bm

// Port D LEDs
#define POWER_LED_BM PIN3_bm
#define LCLICK_LED_BM PIN4_bm
#define SHAKE_LED_BM PIN5_bm

static bool lclick_hold_flag = false;

void io_init(void) {
    // Rotary encoders
    // set PA4, PA5, PA6, PA7 as inputs
    PORTA.DIRCLR = PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;

    // Enable pullup for all 4 pins
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm; // encoder X - A
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm; // encoder X - B
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // encoder Y - A
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // encoder Y - B
    
    // Buttons
    PORTF.DIRCLR = LCLICK_BM | VIRTKBD_BM | QUIT_BM | LCLICKHOLD_BM;
    PORTF.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
    PORTF.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
    PORTF.PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    
    // Debug LED
    PORTF.DIRSET = PIN2_bm;
    PORTF.OUTCLR = PIN2_bm;
    
    // Enclosure LEDs
    PORTD.DIRSET = POWER_LED_BM | LCLICK_LED_BM | SHAKE_LED_BM;
    PORTD.OUTSET = POWER_LED_BM;
    PORTD.OUTCLR = LCLICK_LED_BM;
    PORTD.OUTCLR = SHAKE_LED_BM;
}

void timer_init(void) {
    TCB0.CCMP = 1200; // set interrupt to every 1ms (12MHz / 1000)
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
}

ISR(TCB0_INT_vect) {
    static uint8_t last_state = 0;
    
    // capture the current state of the last 4 pins of PORTA
    uint8_t current_state = (PORTA.IN >> 4) & 0x0F;

    if (current_state != last_state) {
        // toggle LED for debugging
        PORTF.OUTTGL = PIN2_bm;
        
        // process x axis encoder
        uint8_t curr_x = current_state & 0x03;
        uint8_t last_x = last_state & 0x03;
        
        if (curr_x != last_x) {
            if ((last_x & 0x01) ^ ((curr_x & 0x02) >> 1)) {
                delta_next_mouse_move(1, 0);
            } else {
                delta_next_mouse_move(-1, 0);
            }
        }

        // process y axis encoder
        uint8_t curr_y = (current_state >> 2) & 0x03;
        uint8_t last_y = (last_state >> 2) & 0x03;
        
        if (curr_y != last_y) {
            if ((last_y & 0x01) ^ ((curr_y & 0x02) >> 1)) {
                delta_next_mouse_move(0, 1);
            } else {
                delta_next_mouse_move(0, -1);
            }
        }

        last_state = current_state;
    }
    
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

ISR(PORTF_PORT_vect) {
    uint8_t intflags = PORTF.INTFLAGS;
    if (intflags & LCLICK_BM) {
        if (!lclick_hold_flag) {
            set_left_click((PORTF.IN & LCLICK_BM) == 0);
            PORTF.OUTTGL = PIN2_bm;
        }
        PORTF.INTFLAGS = LCLICK_BM;
    }
    
    if (intflags & LCLICKHOLD_BM) {
        if (!lclick_hold_flag) {
            PORTD.OUTSET = LCLICK_LED_BM;
            set_left_click(true);
        } else {
            PORTD.OUTCLR = LCLICK_LED_BM;
            set_left_click(false);
        }
        lclick_hold_flag = !lclick_hold_flag;
        
        PORTF.OUTTGL = PIN2_bm;
        PORTF.INTFLAGS = LCLICKHOLD_BM;
    }
    
    if (intflags & VIRTKBD_BM) {
        if (!(PORTF.IN & VIRTKBD_BM)) {
            set_keyboard_press(usb_kbd_super_k);
        } else {
            set_keyboard_press(usb_kbd_no_keys);
        }
        PORTF.OUTTGL = PIN2_bm;
        PORTF.INTFLAGS = VIRTKBD_BM;
    }
    
    if (intflags & QUIT_BM) {
        if (!(PORTF.IN & QUIT_BM)) {
            set_keyboard_press(usb_kbd_super_q);
        } else {
            set_keyboard_press(usb_kbd_no_keys);
        }
        PORTF.OUTTGL = PIN2_bm;
        PORTF.INTFLAGS = QUIT_BM;
    }
}

int main(void) {
    // Initialize MCC
    SYSTEM_Initialize();
    
    // Initialize USB
    Initialize_USB();
    
    // Initialize IMU
    Initialize_IMU();
    
    // Initialize I/O and timer for rotary encoders
    io_init();
    timer_init();
    
    sei();
    
    bool release_after_shake_flag = false;
    
    while (1) {
        Process_USB_Reports();
        
        if (release_after_shake_flag) {
            set_keyboard_press(usb_kbd_no_keys);
            release_after_shake_flag = false;
        }
        
        if (detect_shake_event(SHAKE_LED_BM)) {
            PORTF.OUTTGL = PIN2_bm;
            set_keyboard_press(usb_kbd_ctrl_z);
            _delay_ms(50); // simulate 50ms keyboard press
            release_after_shake_flag = true;
        }
    }
    return 0;
}