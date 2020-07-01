#include "xc.h"

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config PLL96MHZ = OFF
#pragma config PLLDIV = NODIV
#pragma config SOSCSEL = IO
#pragma config ICS = PGx2       // enables in-circuit debugging

int main(void) {
    // Set RP5/RA0 as an output
    TRISAbits.TRISA0 = 0;
    
    // Unlock the RPINRx and RPORx registers.
    // Performs sequence described in 10.4.4.1 of the datasheet.
    __builtin_write_OSCCONL(OSCCON = 0xBF); // clears IOLOCK bit
    
    // Assign OC2 output to RP5.
    // Refer to Table 10-3 of the datasheet for the interpretation of values
    RPOR2bits.RP5R = 19;
    
    // Lock the RPINRx and RPORx registers
    // Performs sequence described in 10.4.4.1 of the datasheet.
    __builtin_write_OSCCONL(OSCCON | 0x40); // sets IOLOCK bit
    
    // Calculate required values for OC2R and OC2RS to
    // get 20kHz signal w/ 50% dutycycle. Fcyc = 4MHz
    // Refer to section 14.3 of the datasheet
    OC2R = 100;
    OC2RS = 200;
    
    OC2CON1 = 0;
    OC2CON2 = 0;
    OC2CON2bits.SYNCSEL = 0x1F; // OC2 is set as the SYNC source
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCTSEL = 0x7;   // select system clock as clock source.
                                // OC2TMR is the timer
    OC2CON1bits.OCM = 0b110;    // edge-aligned PWM
    
    while (1);
    return 0;
}
