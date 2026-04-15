#include "lib/usb_driver.h"
#include "mcc_generated_files/system/system.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define LCLICK_BM PIN0_bm
#define VIRTKBD_BM PIN1_bm

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
    PORTD.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTD.PIN1CTRL = PORT_PULLUPEN_bm;
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

int main(void) {
    // Initialize MCC
    SYSTEM_Initialize();
    
    PORTF.DIRSET = PIN2_bm;
    PORTF.OUTCLR = PIN2_bm;
    
    // Initialize USB
    Initialize_USB();
    
    // Initialize I/O and timer for rotary encoders
    io_init();
    timer_init();
    
    sei();
    
    while (1) {
        Process_USB_Reports();
        
        // Buttons
        set_left_click((PORTD.IN & LCLICK_BM) == 0);
        
        if ((PORTD.IN & LCLICK_BM) == 0) {
            PORTF.OUTTGL = PIN2_bm;
        }
        
        if ((PORTD.IN & VIRTKBD_BM) == 0) {
            PORTF.OUTTGL = PIN2_bm;
            set_keyboard_press(usb_kbd_super_k);
        } else {
            set_keyboard_press(usb_kbd_no_keys);
        }
    }
    return 0;
}