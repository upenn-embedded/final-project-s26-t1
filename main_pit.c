#include <avr/io.h>
#include <avr/interrupt.h>

/* Global Non-blocking timer (1024 ticks = 1 second) */
volatile uint32_t ms_ticks = 0;

/**
 * Manual PIT Initialization
 * Bypasses MCC and sets registers directly.
 */
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
 * Direct Interrupt Service Routine for PIT
 * This replaces the MCC callback mechanism for higher reliability.
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

int main(void) {
    // Basic System Init (Clocks/Internal references)
    // If you remove SYSTEM_Initialize(), ensure you configure CPU clock manually.
    // SYSTEM_Initialize(); 

    // Setup LED Pin (PF2)
    PORTF.DIRSET = PIN2_bm;
    PORTF.OUTCLR = PIN2_bm; // Start with LED OFF

    // Initialize PIT manually
    PIT_Initialize_Manual();

    // Enable Global Interrupts
    sei();

    while (1) {
        // Your application logic (USB handling, etc.) goes here.
    }
}