#include <xc.h>

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

int adc_an9, adc_an10;       // 10-bit value

void ADC_init(){
    TRISB = 0xC000;     // RB15/AN9 is set as input
                        // RB14/AN10 is set as input
    AD1PCFG = 0xF9FF;   // Set AN9&AN10 as analog
    AD1CON1 = 0x20E4;   // ADC is off
						// Stop module operation in Idle mode
						// Data Output Format bits Integer (0000 00dd dddd dddd)
						// 111 = Internal counter ends sampling and starts conversion
						// Sampling begins immediately after last conversion 
						// SAMP bit is auto-set.
    
    AD1CON2 = 0x0005;   // ADREF+ = AVDD ADREF- = AVSS
                        // Do not scan inputs
                        // A/D is currently filling buffer 0x0-0x7
						// Interrupts at the completion of conversion for
						// every 2nd sample/convert sequence
						// Alternate between MUX A and MUX B
    
    AD1CON3 = 0x0201;   // ADC Clock derived from system clock
						// TAD = 2*TCY, TAD = 135 nSec or 530ksps
    AD1CHS = 0x0009;    // CH0 positive input for MUX A is AN9/RB15
    AD1CHSbits.CH0SB = 0x0A;// CH0 positive input for MUX B is AN10/RB14
    AD1CSSL = 0x0000;
}

int main(void) {
    ADC_init();
    IPC3bits.AD1IP = 5;		// assigning ADC ISR priority
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    AD1CON1bits.ADON = 1;   // turn on ADC
    
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
    OC2RS = adc_an10;
    OC2R = adc_an9;//*/
    /*OC2RS = adc_an9;
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
    adc_an9 = ADC1BUF0;         // Dutycycle
    adc_an10 = ADC1BUF1;        // Frequency
    // Variable Frequency
    adc_an10 = adc_an10 << 2;
    if(adc_an10 >= 3800)
        adc_an10 = 3800;
    adc_an10 = 200 + adc_an10;
    OC2RS = adc_an10;
     //*/
     // Variable Dutycycle
    adc_an10 = adc_an10 >> 7;
    if(adc_an10 >= 19)
        adc_an10 = 19;
    adc_an9 = adc_an9 >> 2;   // check only 8 MSbits
    if(adc_an9 >= 160)
        adc_an9 = 160;
    adc_an9 = 20 + adc_an9;
    OC2R = adc_an9*adc_an10;//*/
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}