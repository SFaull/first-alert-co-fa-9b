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
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 8000000  // Define system clock frequency (8MHz)


typedef struct
{
    uint16_t value;
    uint32_t start;
    uint32_t end;
} note_def_t;


#define NOTE_COUNT  47
const note_def_t song_lut[NOTE_COUNT] = {
    { 54725, 1667, 2130 },
    { 58321, 2222, 2494 },
    { 56955, 2500, 2772 },
    { 56955, 2778, 3241 },
    { 55905, 3333, 3605 },
    { 54725, 3611, 3883 },
    { 54725, 3889, 4161 },
    { 57437, 4167, 4716 },
    { 56955, 4722, 4994 },
    { 56955, 5000, 5272 },
    { 55905, 5278, 5550 },
    { 55905, 5556, 5828 },
    { 54725, 5833, 6105 },
    { 54725, 6389, 6661 },
    { 58321, 6667, 6939 },
    { 56955, 6944, 7216 },
    { 56955, 7222, 7494 },
    { 55905, 7500, 7772 },
    { 55905, 7778, 8050 },
    { 54725, 8056, 8328 },
    { 54725, 8333, 8605 },
    { 52680, 8611, 9161 },
    { 51105, 9167, 9907 },
    { 54725, 10556, 10828 },
    { 54725, 10833, 11105 },
    { 58321, 11111, 11383 },
    { 56955, 11389, 11661 },
    { 56955, 11667, 11939 },
    { 55905, 11944, 12216 },
    { 55905, 12222, 12494 },
    { 54725, 12500, 12772 },
    { 54725, 12778, 13050 },
    { 57437, 13056, 13605 },
    { 56955, 13611, 13883 },
    { 56955, 13889, 14161 },
    { 55905, 14167, 14439 },
    { 55905, 14444, 14716 },
    { 54725, 14722, 14994 },
    { 54725, 15000, 15272 },
    { 58321, 15278, 15828 },
    { 56955, 15833, 16105 },
    { 56955, 16111, 16383 },
    { 55905, 16389, 16939 },
    { 54725, 16944, 17216 },
    { 54725, 17222, 17494 },
    { 55905, 17500, 18050 },
    { 52680, 18056, 18796 },
};

uint8_t lut_index = 0;
uint32_t volatile millis = 0;
uint16_t volatile timer_value = 0;
bool flipflop = false;
bool output_enabled = false;

#define TMR0VAL     200

__inline void setLeds(bool on)
{
    PORTBbits.RB5 = on ? 0 : 1; 
    PORTBbits.RB6 = on ? 0 : 1;  
}

__inline void setTimer(uint16_t val)
{
    TMR1H = (uint8_t)(val >> 8);
    TMR1L = (uint8_t)val;
}

void __interrupt() ISR(void) 
{
    if (INTCONbits.T0IF) 
    {
        INTCONbits.T0IF = 0; // Clear Timer0 interrupt flag
        TMR0 = TMR0VAL; // Reload for next 1ms period
        millis++; // Increment milliseconds counter
    }
    if (PIR1bits.TMR1IF) 
    {
        PIR1bits.TMR1IF = 0; // Clear Timer1 interrupt flag
        setTimer(timer_value);
        if(output_enabled)
            PORTCbits.RC5 ^= 1; // Toggle RC5
    }
}

void main(void) 
{
    // Configure internal oscillator to 8MHz
    OSCCON = 0b01110000; // Select 8MHz internal oscillator
    
    // Configure timer 0 for 1ms ISR
    OPTION_REG = 0b00000100; // Prescaler 1:32 (Timer0 clock = 250kHz with 8MHz Fosc)
    TMR0 = TMR0VAL; // Preload for 1ms overflow (256 - (250000 / 1000))
    INTCONbits.T0IE = 1; // Enable Timer0 interrupt
    
    // Configure timer 1
    T1CON = 0b00000001; // Prescaler 1:1, Timer1 ON
    setTimer(timer_value);
    PIE1bits.TMR1IE = 1; // Enable Timer1 interrupt
    
    // Configure LEDs (RB4, RB5 & RB6) as output
    ANSELHbits.ANS11 = 0; // Ensure AN10/RB4 is a digital pin
    ANSELHbits.ANS11 = 0; // Ensure AN11/RB5 is a digital pin
    TRISBbits.TRISB4 = 0;     // output
    TRISBbits.TRISB5 = 0;     // output
    TRISBbits.TRISB6 = 0;     // output
    PORTBbits.RB4 = 1;        // off
    PORTBbits.RB5 = 1;        // off
    PORTBbits.RB6 = 1;        // off
    
    // Configure Button (RA2) as input with weak pullup enabled
    ANSELbits.ANS2 = 0; // Ensure RA2 is a digital pin
    TRISAbits.TRISA2  = 1; // Set RA2 as input
    WPUAbits.WPUA2 = 1;  // Enable weak pull-up on RA2
    OPTION_REGbits.nRABPU = 0; // Enable individual weak pull-ups
    
    // Configure piezzo output
    TRISCbits.TRISC5 = 0;     // output
    PORTCbits.RC5 = 1;        // off
    
    // enable interrupts
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.GIE = 1; // Enable global interrupts
    
    while (1) 
    {
        if(PORTAbits.RA2) // if button not pressed
        {        
            setLeds(false);
            PIE1bits.TMR1IE = 0; // Disabled Timer1 interrupt
            output_enabled = false;
            lut_index = 0;
            millis = 0;
        }
        else // button is held
        {
            note_def_t* pNoteDef = &song_lut[lut_index];
            
            if(millis < pNoteDef->start)
            {
                PIE1bits.TMR1IE = 0; // buzzer off
                output_enabled = false;
                setLeds(false);
            }
            else if(millis > pNoteDef->start && millis < pNoteDef->end)
            {
                // if the buzzer is off, turn it on at the correct frequency
                if(!PIE1bits.TMR1IE)
                {
                    timer_value = pNoteDef->value;
                    PIE1bits.TMR1IE = 1; // buzzer on
                    setLeds(true);
                    output_enabled = true;
                }
            }
            else if (millis > pNoteDef->end)
            {
                PIE1bits.TMR1IE = 0; // buzzer off
                setLeds(false);
                output_enabled = false;
                lut_index++;
                
                if(lut_index >= NOTE_COUNT)
                    lut_index = 0;
            }
        }
    }
    return;
}
