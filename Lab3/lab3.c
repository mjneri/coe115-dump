/*
 * File:   lab3.c
 * Author: MJ
 *
 * Created on March 8, 2019, 1:57 PM
 */


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

#define DEBOUNCEMAX 1000

// functions
void __attribute__ ((interrupt)) _CNInterrupt(void);
void led1_toggle(void);
void led2_toggle(void);
void led3_toggle(void);
void led4_toggle(void);
void led_binary(int num);

int row1_ispressed, row2_ispressed, row3_ispressed, row4_ispressed;
int col1_ispressed, col2_ispressed, col3_ispressed;
int button_pressed;

int main(void) {
    
    // Configure ports
    // RA0:RA3 - row1:row4 (in)
    // RB0:RB2- col1:col3 (out)
    // RB4:RB5, RB7:RB8 - LED1:LED4 (out)
    AD1PCFG = 0xFFFF;
    TRISA = 0x000F;
    TRISB = 0xFE48;
    LATB = 0xFFFF;
    
    // Configure internal pull-ups
    // RA0 - CN2 (CNPU1 bit 2)
    // RA1 - CN3 (CNPU1 bit 3)
    // RA2 - CN30 (CNPU2 bit 14)
    // RA3 - CN29 (CNPU2 bit 13)
    CNPU1 = CNPU1 | 0x000C;
    CNPU2 = CNPU2 | 0x6000;
    
    // Enable change notification interrupts
    // and clear IRQ flag
    // RA0 - CN2 (CNEN1 bit 2)
    // RA1 - CN3 (CNEN1 bit 3)
    // RA2 - CN30 (CNEN2 bit 14)
    // RA3 - CN29 (CNEN2 bit 13)
    CNEN1 = CNEN1 | 0x000C;
    CNEN2 = CNEN2 | 0x6000;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    
    LATB = 0xFFFE;     // pull-down col1
    while(1){
        if(row1_ispressed){
            if(col1_ispressed){
                led_binary(1);
                while(!PORTAbits.RA0);
                col1_ispressed = 0;
            } 
            else if(col2_ispressed){
                led_binary(2);
                while(!PORTAbits.RA0);
                col2_ispressed = 0;
            } 
            else if(col3_ispressed){
                led_binary(3);
                while(!PORTAbits.RA0);
                col3_ispressed = 0;
            }
            row1_ispressed = 0;
            button_pressed = 0;
        }
        else if(row2_ispressed){
            if(col1_ispressed){
                led_binary(4);
                while(!PORTAbits.RA1);
                col1_ispressed = 0;
            } 
            else if(col2_ispressed){
                led_binary(5);
                while(!PORTAbits.RA1);
                col2_ispressed = 0;
            } 
            else if(col3_ispressed){
                led_binary(6);
                while(!PORTAbits.RA1);
                col3_ispressed = 0;
            }
            row2_ispressed = 0;
            button_pressed = 0;
        }
        else if(row3_ispressed){
            if(col1_ispressed){
                led_binary(7);
                while(!PORTAbits.RA2);
                col1_ispressed = 0;
            } 
            else if(col2_ispressed){
                led_binary(8);
                while(!PORTAbits.RA2);
                col2_ispressed = 0;
            } 
            else if(col3_ispressed){
                led_binary(9);
                while(!PORTAbits.RA2);
                col3_ispressed = 0;
            }
            row3_ispressed = 0;
            button_pressed = 0;
        }
        else if(row4_ispressed){
            if(col1_ispressed){
                led_binary(10);
                while(!PORTAbits.RA3);
                col1_ispressed = 0;
            } 
            else if(col2_ispressed){
                led_binary(0);
                while(!PORTAbits.RA3);
                col2_ispressed = 0;
            } 
            else if(col3_ispressed){
                led_binary(11);
                while(!PORTAbits.RA3);
                col3_ispressed = 0;
            }
            row4_ispressed = 0;
            button_pressed = 0;
        }
        else if(!button_pressed){
            // cycle pulling down of coL1 to col3
            // only one column is pulled down at a time
            if(!PORTBbits.RB0){
                LATBbits.LATB0 = 1;
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 1;
            } else if(!PORTBbits.RB1){
                LATBbits.LATB0 = 1;
                LATBbits.LATB1 = 1;
                LATBbits.LATB2 = 0;
            } else if(!PORTBbits.RB2){
                LATBbits.LATB0 = 0;
                LATBbits.LATB1 = 1;
                LATBbits.LATB2 = 1;
            }
        }
    }
    return 0;
}

void __attribute__ ((interrupt)) _CNInterrupt(void){
    unsigned int debouncecounter = 0;
    
    if( (!PORTAbits.RA0) || (!PORTAbits.RA1) || (!PORTAbits.RA2) || (!PORTAbits.RA3) ){
        while(((!PORTAbits.RA0) || (!PORTAbits.RA1) || (!PORTAbits.RA2) || (!PORTAbits.RA3)) && (debouncecounter < DEBOUNCEMAX))
            debouncecounter++;
        
        if(debouncecounter == DEBOUNCEMAX){
            if(!PORTAbits.RA0)
                row1_ispressed = 1;
            else if(!PORTAbits.RA1)
                row2_ispressed = 1;
            else if(!PORTAbits.RA2)
                row3_ispressed = 1;
            else
                row4_ispressed = 1;
            
            // determine the column that was pulled down when the interrupt was detected
            col1_ispressed = ~PORTBbits.RB0 & 0x0001;
            col2_ispressed = ~PORTBbits.RB1 & 0x0001;
            col3_ispressed = ~PORTBbits.RB2 & 0x0001;
            
            button_pressed = 1;     // determines that a button was pressed
        }
        else{
           row1_ispressed = 0;
           row2_ispressed = 0;
           row3_ispressed = 0;
           row4_ispressed = 0;
        }
    }
    
    IFS1bits.CNIF = 0;
}

void led1_toggle(void){
    LATBbits.LATB4 = !LATBbits.LATB4;   // toggle
    return;
}

void led2_toggle(void){
    LATBbits.LATB5 = !LATBbits.LATB5;   // toggle
    return;
}

void led3_toggle(void){
    LATBbits.LATB7 = !LATBbits.LATB7;   // toggle
    return;
}

void led4_toggle(void){
    LATBbits.LATB8 = !LATBbits.LATB8;   // toggle
    return;
}

void led_binary(int num){
    num = ~num;
    LATBbits.LATB4 = num;
    num = num >> 1;
    LATBbits.LATB5 = num;
    num = num >> 1;
    LATBbits.LATB7 = num;
    num = num >> 1;
    LATBbits.LATB8 = num;
    return;
}