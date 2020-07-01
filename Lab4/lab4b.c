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
int dir;            // 0 = left, 1 = right

void ADC_init(){
    TRISB = 0x8000;
    AD1PCFG = 0xFDFF;   // AN9 is set as analog
    AD1CON1 = 0x20E4;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0201;
    AD1CHS = 0x0009;
    AD1CSSL = 0x0000;
}

void TMR1_init(){
    T1CON = 0x0000;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11; // FCYC/256 = 15.625kHz
    T1CONbits.TGATE = 0;
    T1CONbits.TSIDL = 0;    // stop at idle
}

void delay(unsigned int cyc){
    PR1 = cyc;
    T1CONbits.TON = 1;
    while(TMR1 != PR1);
    T1CONbits.TON = 0;
    TMR1 = 0;
}

int main(void) {
    ADC_init();             // initialize ADC
    TMR1_init();            // initialize tmr1  
    TRISA = 0x0001;         // set RA0 as input
    LATB = 0x7FFF;
    
    // initialize LEDs
    LATB = 0xFFC0;
    delay(31250);   // 2 sec delay
    LATB = 0xFFFF;
    delay(31250);   // 2 sec delay
    
    // Enable ADC interrupts
    IPC3bits.AD1IP = 5;		// assigning ADC ISR priority
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    AD1CON1bits.ADON = 1;   // turn on ADC
    
    // Enable change notification interrupts
    // and clear IRQ flag
    // RA0 - CN2 (CNEN1 bit 2)
    CNEN1 = CNEN1 | 0x0004;
    IPC4bits.CNIP = 6;          // assign CN ISR priority
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    
    LATB = 0x7FFE;
    while(1){
        delay(adcvalue);        // [.125, 1] sec delay
                                // [1953, 15625] cycles
        if(!dir){
            if(!LATBbits.LATB5){
                dir = 1;
                continue;
            }
            LATB = (LATB << 1) | 0x7FC1;
            if(!LATBbits.LATB5)
                dir = 1;
        }
        else{
            if(!LATBbits.LATB0){
                dir = 0;
                continue;
            }
            LATB = (LATB >> 1) | 0x7FE0;
            if(!LATBbits.LATB0)
                dir = 0;
        }
    }
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    adcvalue = ADC1BUF0;
    adcvalue = adcvalue << 4;
    if(adcvalue >= 13672)
        adcvalue = 13672;
    adcvalue = 1953 + adcvalue;
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void){
    dir = (~dir) & 0x0001;
    while(!PORTAbits.RA0);
    IFS1bits.CNIF = 0;
}