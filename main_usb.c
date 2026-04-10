#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_common/usb_core_transfer.h"
#include "mcc_generated_files/usb/usb_peripheral/usb_peripheral_read_write.h"
#include <avr/interrupt.h>
#include <avr/io.h>

/* Global Non-blocking timer (1024 ticks = 1 second) */
volatile uint32_t ms_ticks = 0;

/* HID Report Buffers */
uint8_t kbd_press[9]   = {0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00}; 
uint8_t kbd_release[9] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
uint8_t mouse_move[4]  = {0x02, 0x00, 0x32, 0x00}; // (report ID, Buttons, X-axis, Y-axis)

/* Define Pipe Structure for Endpoint 1 IN */
USB_PIPE_t hidPipe = {
    .address = 1,              
    .direction = USB_EP_DIR_IN 
};

void PIT_Initialize_Manual(void) {
    // 1. Ensure the 32.768 kHz Internal Oscillator is running
    // This is usually handled in SYSTEM_Initialize, but we'll ensure it here.
    
    // 2. Configure RTC Clock Selection to Internal 32.768 kHz
    RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;

    // 3. Wait for PIT to be ready (synchronization)
    while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);

    // 4. Set Periodic Interrupt Timer: 32 Cycles, Enable PIT
    // PERIOD = CYC32 (32 RTC cycles)
    RTC.PITCTRLA = RTC_PERIOD_CYC32_gc | RTC_PITEN_bm;

    // 5. Enable the Periodic Interrupt (PI)
    RTC.PITINTCTRL = RTC_PI_bm;
}

/**
 * PIT Interrupt Callback
 * With RTC Cycles = 32, this executes 1024 times per second.
 */
ISR(RTC_PIT_vect) {
    ms_ticks++;

    // Toggle PF2 (LED0) every 512 ticks (~0.5s)
    if ((ms_ticks % 512) == 0) {
        PORTF.OUTTGL = PIN2_bm;
    }

    /* IMPORTANT: Clear the interrupt flag by writing a '1' to it */
    RTC.PITINTFLAGS = RTC_PI_bm;
}

int main(void)
{
    // 1. Initialize System (Clocks, Ports, PIT via MCC)
    SYSTEM_Initialize();
    
    // Ensure PF2 is an output for the LED
    PORTF.DIRSET = PIN2_bm; 
    PORTF.OUTCLR = PIN2_bm;

    // 3. Initialize and start the USB stack
    USBDevice_Initialize();
    USB_Start();
    
    PIT_Initialize_Manual();

    // 4. Enable Global Interrupts
    sei();

    uint32_t last_action_time = 0;
    uint8_t current_state = 0;

    while (1)
    {
        // Maintain USB connection
        RETURN_CODE_t usb_status = USBDevice_Handle();

        if (usb_status == SUCCESS)
        {
            if (!USB_PipeStatusIsBusy(hidPipe))
            {
                uint32_t now = ms_ticks;

                switch(current_state) {
                    case 0: // Keyboard Press every 1 second (1024 ticks)
                        if (now - last_action_time >= 1024) {
                            USB_TransferWriteStart(hidPipe, kbd_press, 9, false, NULL);
                            last_action_time = now;
                            current_state = 1;
                        }
                        break;

                    case 1: // Keyboard Release after ~50ms (51 ticks)
                        if (now - last_action_time >= 51) {
                            USB_TransferWriteStart(hidPipe, kbd_release, 9, false, NULL);
                            last_action_time = now;
                            current_state = 2;
                        }
                        break;

                    case 2: // Mouse Move every 0.5 seconds (512 ticks)
                        if (now - last_action_time >= 512) {
                            USB_TransferWriteStart(hidPipe, mouse_move, 4, false, NULL);
                            last_action_time = now;
                            current_state = 0; 
                        }
                        break;
                }
            }
        }
    }
}