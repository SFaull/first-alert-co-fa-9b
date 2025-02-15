/*
 * File:   main.c
 * Author: sfaull
 *
 * Created on 15 February 2025, 16:14
 */


// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#include <xc.h>

#define _XTAL_FREQ 8000000  // Define system clock frequency (8MHz)

void main(void) 
{
    // Configure internal oscillator to 8MHz
    OSCCON = 0b01110000; // Select 8MHz internal oscillator

    // Configure LEDs (RB4, RB5 & RB6) as output
    ANSELHbits.ANS11 = 0; // Ensure AN10/RB4 is a digital pin
    ANSELHbits.ANS11 = 0; // Ensure AN11/RB5 is a digital pin
    TRISB4 = 0;     // output
    TRISB5 = 0;     // output
    TRISB6 = 0;     // output
    RB4 = 1;        // off
    RB5 = 1;        // off
    RB6 = 1;        // off
    
    // Configure Button (RA2) as input with weak pullup enabled
    ANSELbits.ANS2 = 0; // Ensure RA2 is a digital pin
    TRISA2 = 1; // Set RA2 as input
    WPUA2 = 1;  // Enable weak pull-up on RA2
    nRABPU = 0; // Enable individual weak pull-ups
    
    // Configure piezzo output
    TRISC5 = 0;     // output
    RC5 = 1;        // off
    
    while (1) 
    {
        if(PORTAbits.RA2) // if button not pressed, flash the LEDs
        {        
            PORTBbits.RB5 = 0;  // Turn on LED on RB5
            PORTBbits.RB6 = 0;  // Turn on LED on RB6
            __delay_ms(50);

            PORTBbits.RB5 = 1;  // Turn off LED on RB5
            PORTBbits.RB6 = 1;  // Turn off LED on RB6
            __delay_ms(200);
        }
        else // button is held
        {
            // buzz the piezzo
            __delay_us(200);
            PORTCbits.RC5 = !PORTCbits.RC5;
        }
    }
    return;
}
