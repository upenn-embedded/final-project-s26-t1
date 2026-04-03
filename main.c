/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 

#define F_CPU 16000000UL
#define joystick_pin_x DDRC5
#define joystick_pin_y DDRC4

#define LED1_pin DDRD0
#define LED2_pin DDRD1

#define clear_pin DDRC2


#include <avr/io.h>
#include <avr/interrupt.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "uart.h"
#include <util/delay.h>

#include <stdio.h>

uint16_t joystick_center = 500;

uint16_t color = 0;

int cursor_x = 10;
int cursor_y = 30;

void ADC_init(void) {
    ADMUX = (1 << REFS0); // AVcc reference, ADC0 default

    ADCSRA = (1 << ADEN)  // Enable ADC
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

void Initialize()
{
	lcd_init();

    // Set paddle control pins as analog input
    ADC_init();

    // Set LED pins as output
    DDRD |= (1 << LED1_pin) | (1 << LED2_pin);

    // Set clear pin as input with pull-up
    DDRC &= ~(1 << clear_pin);
    PORTC |= (1 << clear_pin);

    // Initialize LEDs to off
    PORTD &= ~((1 << LED1_pin) | (1 << LED2_pin));
}



uint16_t ADC_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // select channel

    ADCSRA |= (1 << ADSC); // start conversion
    while (ADCSRA & (1 << ADSC)); // wait

    return ADC;
}

int get_joystick_x_value() {
    int value = ADC_read(joystick_pin_x);

    return (value - joystick_center )/ 50;
}

int get_joystick_y_value() {
    int value = ADC_read(joystick_pin_y);

    return (value - joystick_center )/ 50;
}

void update_cursor(int new_x, int new_y){
    if (new_y > LCD_HEIGHT) {
        new_y = LCD_HEIGHT;
    }
    if (new_y < 0) {
        new_y = 0; 
    }

    if (new_x > LCD_WIDTH) {
        new_x = LCD_WIDTH;
    }
    if (new_x < 0) {
        new_x = 0;
    }

    LCD_drawLine(cursor_x, cursor_y, new_x, new_y, color);

    cursor_x = new_x;
    cursor_y = new_y;
}

int get_clear_button() {
    return !(PINC & (1 << clear_pin));
}



int main(void)
{
	Initialize();
    LCD_setScreen(rgb565(255, 255, 255)); 

		
    while (1) 
    {
        if (get_clear_button()) {
            LCD_setScreen(rgb565(255, 255, 255)); // Clear screen
        }

        int get_x_change = get_joystick_x_value();
        int get_y_change = get_joystick_y_value();

        int new_x = cursor_x + get_x_change;
        int new_y = cursor_y + get_y_change;

        update_cursor(new_x, new_y);

        _delay_ms(20); // Delay for a short period to display speed
        
    }
}