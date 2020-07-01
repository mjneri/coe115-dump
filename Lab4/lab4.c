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
int full;

void ADC_init(){
    TRISB = 0x8000;     // RB15/AN9 is set as input
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
    ADC_init();             // initialize ADC
    IPC3bits.AD1IP = 5;		// assigning ADC ISR priority
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    AD1CON1bits.ADON = 1;   // turn on ADC
    
    while(1){
        switch(adcvalue){
            case 0: LATB = 0xFFFF;
            break;
            case 1: LATB = 0xFFFE;
            break;
            case 2: LATB = 0xFFFC;
            break;
            case 3: LATB = 0xFFF8;
            break;
            case 4: LATB = 0xFFF0;
            break;
            case 5: LATB = 0xFFE0;
            break;
            case 6: LATB = 0xFFC0;
            break;
            case 7: 
                if(full == 1023)
                    LATB = 0xFE40;
                else
                    LATB = 0xFF40;
            break;
        }
    }
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    adcvalue = ADC1BUF0;
    full = ADC1BUF0;
    adcvalue = adcvalue >> 7;   // check only 3 MSBits
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}