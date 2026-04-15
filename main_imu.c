#define F_CPU 4000000UL

#include "mcc_generated_files/system/system.h"
#include <util/delay.h>
#include "lib/lsm6dso_driver.h"

int main(void) {
    // initialize UART
    SYSTEM_Initialize();
    
    Initialize_IMU();
    
    int shaking_ticks = 0;
    
    
    while(1) {
        bool shaken = detect_shake_event();
        
        if (shaken) {
            printf("Detected!\n");
        }
        
        _delay_ms(100);
    }
    
    return 0;
}