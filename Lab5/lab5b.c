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

int adcvalue;       // 10-bit value

void ADC_init(){
    TRISBbits.TRISB15 = 1;     // RB15/AN9 is set as input
    AD1PCFG = 0xFDFF;   
    AD1CON1 = 0x20E4;   // ADC is off
						// Stop module operation in Idle mode
						// Data Output Format bits Integer (0000 00dd dddd dddd)
						// 111 = Internal counter ends sampling and starts conversion
						// Sampling begins immediately after last conversion 
						// SAMP bit is auto-set.
    
    AD1CON2 = 0x0000;   // ADREF+ = AVDD ADREF- = AVSS
						// Do not scan inputs
						// A/D is currently filling buffer 0x0-0x7
						// Interrupts at the completion of conversion for
						// each sample/convert sequence
						// Always uses MUX A input mux settings
    
    AD1CON3 = 0x0201;   // ADC Clock derived from system clock
						// TAD = 2*TCY, TAD = 135 nSec or 530ksps
    AD1CHS = 0x0009;    // CH0 positive input is AN9
    AD1CSSL = 0x0000;
}

int main(void) {
    ADC_init();
    IPC3bits.AD1IP = 5;		// assigning ADC ISR priority
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    AD1CON1bits.ADON = 1;   // turn on ADC
    
    // Set RP5/RA0 as an output
    TRISBbits.TRISB14 = 0;
    
    // Unlock the RPINRx and RPORx registers.
    // Performs sequence described in 10.4.4.1 of the datasheet.
    __builtin_write_OSCCONL(OSCCON = 0xBF); // clears IOLOCK bit
    
    // Assign OC2 output to RP5.
    // Refer to Table 10-3 of the datasheet for the interpretation of values
    RPOR7bits.RP14R = 19;
    
    // Lock the RPINRx and RPORx registers
    // Performs sequence described in 10.4.4.1 of the datasheet.
    __builtin_write_OSCCONL(OSCCON | 0x40); // sets IOLOCK bit
    
    // Calculate required values for OC2R and OC2RS to
    // get 20kHz signal w/ 50% dutycycle. Fcyc = 4MHz
    // Refer to section 14.3 of the datasheet
    OC2RS = 200;
    OC2R = adcvalue;//*/
    /*OC2RS = adcvalue;
    OC2R = OC2RS >> 1;//*/
    
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

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    adcvalue = ADC1BUF0;
    // Variable Dutycycle
    adcvalue = adcvalue >> 2;   // check only 8 MSbits
    if(adcvalue >= 160)
        adcvalue = 160;
    adcvalue = 20 + adcvalue;
    OC2R = adcvalue;//*/
    
    // Variable Frequency
    /*adcvalue = adcvalue << 4;
    if(adcvalue >= 3800)
        adcvalue = 3800;
     adcvalue = 200 + adcvalue;
     OC2RS = adcvalue;
     OC2R = OC2RS >> 1;
     //*/
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}