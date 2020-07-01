#include <p24FJ64GB002.h>

#include "xc.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma config FWDTEN = OFF     // disables WDT
#pragma config JTAGEN = OFF     // JTAG port is disabled
#pragma config POSCMOD = NONE   // Primary osc is disabled
#pragma config OSCIOFNC = ON    // OSCO functions as port I/O
#pragma config FCKSM = CSDCMD   // Clock switching & Clock monitor disabled
#pragma config FNOSC = FRCPLL   // FRC w/ postscaler and PLL module
#pragma config PLL96MHZ = OFF   // 96MHz PLL is enabled by the user, not automatically
#pragma config PLLDIV = NODIV   // Osc input used directly
#pragma config SOSCSEL = IO     // SOSC pins are used for I/O (RA4, RB4)
#pragma config ICS = PGx2       // enables in-circuit debugging

#define BUFSIZE 128

// Global Variables
static char Rxbuffer[BUFSIZE];
static char prevRx[BUFSIZE];
static char playing[BUFSIZE];
static unsigned int adcvalue, prev_adcvalue;
static int rx_recvd = 0;	// flag
static int volume_mode = 0; // for displaying volume levels on LCD

void PIC_init(void);
void ADC_init(void);
void PB_init(void);
void UART_init(void);
void Timer_init(void);

void UART_Read(void);
void UART_Write(char *data);
unsigned long millis(void);

int main(void){
	// RA0: Play/Pause
	// RA1: Previous
	// RA2: Next
	// RB15: Volume
    // Message Headers: INIT, DONE, ERRR, SONG
    unsigned long start = 0;
    char buf[BUFSIZE];
    PIC_init();
    
    /*while(1){
        UART_Read();
        if(strcmp(prevRx, Rxbuffer) != 0)
            LCD_write(Rxbuffer);
    }*/
    
    while(1){
        if(!PORTAbits.RA0){
            UART_Write("PLAYPAUSE\r\n");
            while(!PORTAbits.RA0);
            volume_mode = 0;
        }
        else if(!PORTAbits.RA1){
            UART_Write("PREV\r\n");
            while(!PORTAbits.RA1);
            volume_mode = 0;
        }
        else if(!PORTAbits.RA2){
            UART_Write("NEXT\r\n");
            while(!PORTAbits.RA2);
            volume_mode = 0;
        }
        else if(adcvalue != prev_adcvalue){
            if(!volume_mode){
                volume_mode = 1;
                LCD_clearLine2();
                sprintf(buf, "Vol: %03d%%", adcvalue);
                LCD_print(buf);
                start = millis();
            }
            else{
                sprintf(buf, "%03d%%", adcvalue);
                LCD_setcursor(2, 5);
                LCD_print(buf);
                start = millis();
            }
        }                
        else if((adcvalue == prev_adcvalue) && volume_mode == 1){
            sprintf(buf, "%03d%%", adcvalue);
            LCD_setcursor(2, 5);
            LCD_print(buf);
        	if((millis() - start) >= 5000){
        		sprintf(buf, "%dVOL\r\n", adcvalue);
        		UART_Write(buf);
                LCD_write(prevRx);
                volume_mode = 0;
        	}
        }
        else{
            if(!volume_mode){
                UART_Read();
                if(strcmp(prevRx, Rxbuffer) != 0){
                    LCD_write(Rxbuffer);
                    strcpy(playing, Rxbuffer);
                }
            }
        }
    }
    return 0;
}

/* Interrupt functions */
void __attribute__ ((interrupt, auto_psv)) _U1ErrInterrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    // Error checks
    if(U1STAbits.OERR)
        U1STAbits.OERR = 0;
    
    __builtin_disi(0x0000);     // enable interrupts
    IFS4bits.U1ERIF = 0;        // clear flag
}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    prev_adcvalue = adcvalue;
    adcvalue = (ADC1BUF0 >> 5)/0.31;
    if(adcvalue == 0)
        adcvalue = 0;
    else if(adcvalue < 5)
        adcvalue = 5;
    else if( (adcvalue >= 5) && (adcvalue < 10) )
        adcvalue = 10;
    else if( (adcvalue >= 10) && (adcvalue < 15) )
        adcvalue = 15;
    else if( (adcvalue >= 15) && (adcvalue < 20) )
        adcvalue = 20;
    else if( (adcvalue >= 20) && (adcvalue < 25) )
        adcvalue = 25;
    else if( (adcvalue >= 25) && (adcvalue < 30) )
        adcvalue = 30;
    else if( (adcvalue >= 30) && (adcvalue < 35) )
        adcvalue = 35;
    else if( (adcvalue >= 35) && (adcvalue < 40) )
        adcvalue = 40;
    else if( (adcvalue >= 40) && (adcvalue < 45) )
        adcvalue = 45;
    else if( (adcvalue >= 45) && (adcvalue < 50) )
        adcvalue = 50;
    else if( (adcvalue >= 50) && (adcvalue < 55) )
        adcvalue = 55;
    else if( (adcvalue >= 55) && (adcvalue < 60) )
        adcvalue = 60;
    else if( (adcvalue >= 60) && (adcvalue < 65) )
        adcvalue = 65;
    else if( (adcvalue >= 65) && (adcvalue < 70) )
        adcvalue = 70;
    else if( (adcvalue >= 70) && (adcvalue < 75) )
        adcvalue = 75;
    else if( (adcvalue >= 75) && (adcvalue < 80) )
        adcvalue = 80;
    else if( (adcvalue >= 80) && (adcvalue < 85) )
        adcvalue = 85;
    else if( (adcvalue >= 85) && (adcvalue < 90) )
        adcvalue = 90;
    else if( (adcvalue >= 90) && (adcvalue < 95) )
        adcvalue = 95;
    else if( (adcvalue >= 95) && (adcvalue < 100) )
        adcvalue = 100;
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}


/* UART Read and write functions */
void UART_Read(void){  
    __builtin_disi(0x3FFF);	// Disable interrupts
    int j = 0;
    unsigned long start = millis();
    
    memset(prevRx, 0, sizeof(prevRx));	// Initialize prevRx contents to \0
    strcpy(prevRx, Rxbuffer);
    
	// check if data has been received
	if(U1STAbits.URXDA){
        memset(Rxbuffer, 0, sizeof(Rxbuffer));	// initialize Rxbuffer to \0
        while( (millis() - start) < 200 ){		// keep looping as long as not more than 200ms has passed between bytes sent
            if(U1STAbits.URXDA){
				// If a byte is received, store it in Rxbuffer
                Rxbuffer[j] = U1RXREG;
                j++;
                start = millis();
            }
        }
    }
    __builtin_disi(0x0000);
    return;
}

void UART_Write(char *data){
     __builtin_disi(0x3FFF);     // disable interrupts
    unsigned int len = strlen(data);
    unsigned int i = 0;
    for(i = 0; i < len; i++){
        while(U1STAbits.UTXBF);	// Loop while the transmit buffer is full
        U1TXREG = data[i];
    }
    while(!U1STAbits.TRMT);     // wait for transmission to finish
    /*U1STAbits.UTXBRK = 1;       // send break
    U1TXREG = 0xFF;
    while(U1STAbits.UTXBRK);*/
    __builtin_disi(0x0000);     // enable interrupts
    return;
}

unsigned long millis(void){
	// Similar to millis() function in arduino
	unsigned long temp = TMR3;
	temp = (temp << 16) | TMR2;
	temp = temp/500;
	return temp;
}

/* Initialization functions */
void PIC_init(void){
    // Ports Config:
    // LCD: RB5:RB0 -> output
    AD1PCFG = 0xFFFF;       // Set all pins as digital
    TRISA = 0x0000;			// Set TRISA to output
    TRISB = 0x0000;         // Set TRISB to output

    // Call initialization functions
    LCD_init();
    Timer_init();
    ADC_init();
    PB_init();
    UART_init();
    return;
}

void ADC_init(void){
    TRISB = TRISB | 0x8000;     // RB15/AN9 is set as input
    AD1PCFG = 0xFDFF;   // set RB15/AN9 as analog
    AD1CON1 = 0x20E4;   // ADC is off
						// Stop module operation in Idle mode
						// Data Output Format bits Integer (0000 00dd dddd dddd)
						// 111 = Internal counter ends sampling and starts conversion
						// Sampling begins immediately after last conversion 
						// SAMP bit is auto-set.
    
    AD1CON2 = 0x0000;   // ADREF+ = AVDD ADREF- = AVSS
						// Do not scan inputs
						// A/D is currently filling buffer 0x0-0x7
						// Interrupts at the completion of conversion
						// every sample/convert sequence
						// Always uses MUX A input mux settings
    
    AD1CON3 = 0x1F3F;   // ADC Clock derived from system clock
						// TAD = 2*TCY, TAD = 135 nSec or 530ksps
    AD1CHS = 0x0009;    // CH0 positive input is AN9
    AD1CSSL = 0x0000;

    // Enable interrupts
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    AD1CON1bits.ADON = 1;	// Enable ADC
    return;
}

void UART_init(void){
    // Use UART1
    // Set UART Rx & Tx pins
    __builtin_write_OSCCONL(OSCCON = 0xBF);     // clear IOLOCK
    RPINR18bits.U1RXR = 8;      // assign U1RX to pin RP8/RB8
    RPOR4bits.RP9R = 3;         // assign U1TX to pin RP9/RB9
    __builtin_write_OSCCONL(OSCCON | 0x40);     // set IOLOCK
    
    U1BRG = 25;             // Set baudrate to 9600bps
    U1MODEbits.USIDL = 1;   // Stop in idle
    U1MODEbits.UEN = 0;     // Tx&Rx enabled, CTS/RTS unused
    U1MODEbits.LPBACK = 0;  // Loopback mode: RTS/CTS unused
    U1MODEbits.PDSEL = 0;   // 8bit data, no parity
    U1MODEbits.STSEL = 0;   // 1 stop bit
    U1MODEbits.RXINV = 0;   // idle state is 1
    
    // enable&setup interrupts
    IFS4bits.U1ERIF = 0;
    IEC4bits.U1ERIE = 0;    // Rx interrupt
    U1STAbits.URXISEL = 0;  // interrupt when char recvd
    
    U1MODEbits.UARTEN = 1;  // enable UART
    U1STAbits.UTXEN = 1;
}

void PB_init(void){
	// Pushbutton: RA0,1,2 -> input
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;

    // Enable change notification interrupts
    // and clear IRQ flag
    // RA1 - CN3 (CNEN1 bit 3)
    // RA2 - CN30 (CNEN2 bit 14)
    // RA3 - CN29 (CNEN2 bit 13)
    // RA4 - CN0 (CNEN1 bit 0)
    /*CNEN1 = CNEN1 | 0x0009;
    CNEN2 = CNEN2 | 0x6000;
    IPC4bits.CNIP = 3;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;*/
    //LATB = LATB | 0x0380;      // no input from keypad yet
}

void Timer_init(void){
	// TMR3:TMR2
    T2CON = 0x0000;         // clear T2CON
    TMR2 = 0;               // clear TMR2
    TMR3 = 0;               // clear TMR3
    T2CONbits.T32 = 1;      // operate TMR2 as a 32-bit timer
    T2CONbits.TCS = 0;      // Clock source is FRC
    T2CONbits.TCKPS = 1;    // FRC/8 = 500kHz
    T2CONbits.TGATE = 0;    // Gated time acc is disabled
    T2CONbits.TSIDL = 0;    // Continue in idle mode
    
    // Set PR3:PR2 value to max value
    PR2 = 0xFFFF;
    PR3 = 0xFFFF;
    T2CONbits.TON = 1;      // Turn on timer
    return;
}