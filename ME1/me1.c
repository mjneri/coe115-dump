#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "me1_functions.h"

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

#define START 1
#define MODESELECT 2
#define GAMEMODE1 3
#define GAMEMODE2 4
#define GAMEOVER 5

#define EASY 1
#define AVE 2
#define HARD 3

// Define globals
int state;          // game state
int row_pressed, col_pressed, keypad_pressed;
int key;            // what key was pressed on the keypad (*=0xA, #=0xB)
int difficulty;
int adcvalue;       
int score;

int gametimer_current, freq_counter;
int dutycycle;      // changing LED brightness
int dc_dir;

// Define functions
void PIC_init(void);                // initialize pins, stack pointer, etc.
void TMR1_init(void);               // initialize TMR1 for delays
void GAME_Timer(long seconds);      // initialize & start TMR2 for Game timer
void TMR4_init(void);               // initialize TMR4 for LED PWM
void TMR5_init(void);               // initialize TMR5 for RNG
void OCx_init(void);                // initialize OC1&2 for LED PWM
void ADC_init(void);                // initialize ADC

int main(void) {
    PIC_init();
    TMR1_init();
    TMR5_init();
    LCD_init();

    // #########################################################################
    // Start
    state = START;
    LCD_print("   WELCOME TO\n <ENDER>'s GAME");
    LCD_send(DISPLAY | DISPLAY_ON);     // Send Display on w/ cursor turned off
    while(state == START){              // Poll RA0 for pushbutton state
        if(!PORTAbits.RA0){
            while(!PORTAbits.RA0);
            state = MODESELECT;
        }
    }
    // #########################################################################
    
    // #########################################################################
    // Mode select
    LCD_send(CLEARDISPLAY);
    LCD_print("   GAME MODE\n     SELECT");
    
    T1CONbits.TCKPS = 0b11;     // Prescale FRC to 15.625kHz
    delay(15625);               // 1 second delay
    T1CONbits.TCKPS = 0;        // Prescale FRC back to 4MHz
    
    LCD_send(CLEARDISPLAY);
    LCD_print("1-Key Input Mode\n2-Multi Choice");
    int i = 0b110;          // for cycling pulldown between columns
    LATBbits.LATB7 = 0;     // pull down col1
    while(1){
        key = keypad_in(row_pressed, col_pressed);
        if(key != -1){
            switch(key){
                case 1: state = GAMEMODE1;
                        break;
                case 2: state = GAMEMODE2;
                        break;
            }
            
            row_pressed = 0;
            col_pressed = 0;
            keypad_pressed = 0;
            if(state == MODESELECT)
                continue;
            else
                break;
        }
        else if(!keypad_pressed){
            if( (i&0x0007) == 0b011 )
                i = 0b110;
            else
                i = (i << 1) | 1;
            
            LATBbits.LATB7 = i & 0x0001;
            LATBbits.LATB8 = (i & 0x0002) >> 1;
            LATBbits.LATB9 = (i & 0x0004) >> 2;
        }
    }
    LATB = LATB | 0x0380;   // "Disable" keypad
    // #########################################################################
    
    // #########################################################################
    // Difficulty select
    LCD_send(CLEARDISPLAY);
    LCD_print("Game Mode ");
    if(state == GAMEMODE1)
        LCD_print("1");
    else if(state == GAMEMODE2)
        LCD_print("2");
    
    LCD_print("\nDifficulty: ");
    AD1CON1bits.ADON = 1;   // turn on ADC
    while(1){
        LCD_send(SETDDRAMADDR | 0x004C);
        if(adcvalue == 1)
            LCD_print("Easy");
        else if(adcvalue == 2)
            LCD_print("Ave ");
        else if(adcvalue == 3)
            LCD_print("Hard");
        
        if(!PORTAbits.RA0){     // poll pushbutton
            while(!PORTAbits.RA0);
            difficulty = adcvalue;
            break;
        }
    }
    //AD1CON1bits.ADON = 0;   // turn off ADC
    // #########################################################################
    
    // #########################################################################
    // Game Proper
    char str[33];
    int rand_op, rand_A, rand_B;
    int correct_ans, submitted_ans, correct_letter;
    int b_pos, c_pos, lcd_pos;
    TMR4_init();
    
    if(difficulty == EASY)       
        GAME_Timer(10);
    else if(difficulty == AVE)
        GAME_Timer(15);    
    else if(difficulty == HARD)
        GAME_Timer(20);
    
    T4CONbits.TON = 1;      // Turn on timer4
    OCx_init();
    
    // Key input mode
    while(state == GAMEMODE1){
        LCD_send(CLEARDISPLAY);
        LCD_send(DISPLAY | DISPLAY_ON | DISPLAY_CURSOR);
        // generate operands
        srand(TMR5);
        rand_op = (rand() % difficulty) + 1;
        srand(TMR5 + TMR4);
        rand_A = (rand() % 9) + 1;
        srand(TMR5 + TMR4 + TMR2);
        rand_B = (rand() % 9) + 1;
        // Print question
        switch(rand_op){
            case 1: correct_ans = rand_A + rand_B;
                    sprintf(str, "Q: %d + %d\nA: ", rand_A, rand_B);
                    break;
            case 2: correct_ans = rand_A - rand_B;
                    sprintf(str, "Q: %d - %d\nA: ", rand_A, rand_B);
                    break;
            case 3: correct_ans = rand_A * rand_B;
                    sprintf(str, "Q: %d * %d\nA: ", rand_A, rand_B);
                    break;
        }
        LCD_print(str);
        
        lcd_pos = 0;
        i = 0b110;
        LATBbits.LATB7 = 0;     // pull down col1
        int is_negative = 0;
        while(state == GAMEMODE1){
            if(!PORTAbits.RA0){
                while(!PORTAbits.RA0 && (state == GAMEMODE1));
                break;
            }
            key = keypad_in(row_pressed, col_pressed);
            if(key != -1){
                if(key == 0xA){ // *
                    if(lcd_pos == 0)
                        lcd_pos = 0;
                    else{
                        lcd_pos--;
                        LCD_send(SETDDRAMADDR | (0x0043 + lcd_pos));
                        LCD_print(" ");
                        LCD_send(SETDDRAMADDR | (0x0043 + lcd_pos));
                        // CHANGE STORED ANSWER
                        // divide stored answer by 10
                        if(lcd_pos == 0)
                            is_negative = 0;
                        else
                            submitted_ans = submitted_ans/10;
                    }
                }
                else if(key == 0xB){
                    if(lcd_pos == 0)
                        is_negative = 1;
                    LCD_print("-");
                    lcd_pos++;
                }
                else{
                    if(key == 1)
                        LCD_print("1");
                    else if(key == 2)
                        LCD_print("2");
                    else if(key == 3)
                        LCD_print("3");
                    else if(key == 4)
                        LCD_print("4");
                    else if(key == 5)
                        LCD_print("5");
                    else if(key == 6)
                        LCD_print("6");
                    else if(key == 7)
                        LCD_print("7");
                    else if(key == 8)
                        LCD_print("8");
                    else if(key == 9)
                        LCD_print("9");
                    else if(key == 0)
                        LCD_print("0");
                    submitted_ans = submitted_ans*10 + key;
                    lcd_pos++;
                }
                row_pressed = 0;
                col_pressed = 0;
                keypad_pressed = 0;
            }
            else if(!keypad_pressed){
                if( (i&0x0007) == 0b011 )
                    i = 0b110;
                else
                    i = (i << 1) | 1;
                LATBbits.LATB7 = i & 0x0001;
                LATBbits.LATB8 = (i & 0x0002) >> 1;
                LATBbits.LATB9 = (i & 0x0004) >> 2;
            }
        }
        LATB = LATB | 0x0380;   // "Disable" keypad
        
        // Check if the answer is correct
        // do something
        if(is_negative == 1)
            submitted_ans = submitted_ans*(-1);
        if((submitted_ans == correct_ans) && (state == GAMEMODE1))
            score += rand_op;
        submitted_ans = 0;
    }
    
    // Mulcho
    while(state == GAMEMODE2){
        int rand_op_orig;
        LCD_send(DISPLAY | DISPLAY_ON | DISPLAY_CURSOR);
        LCD_send(CLEARDISPLAY);
        // generate operands
        srand(TMR5);
        rand_op = (rand() % difficulty) + 1;
        srand(TMR5 + TMR4);
        rand_A = (rand() % 9) + 1;
        srand(TMR5 + TMR4 + TMR2);
        rand_B = (rand() % 9) + 1;
        rand_op_orig = rand_op;
        // Print question
        switch(rand_op){
            case 1: correct_ans = rand_A + rand_B;
                    sprintf(str, "Q: %d + %d", rand_A, rand_B);
                    break;
            case 2: correct_ans = rand_A - rand_B;
                    sprintf(str, "Q: %d - %d", rand_A, rand_B);
                    break;
            case 3: correct_ans = rand_A * rand_B;
                    sprintf(str, "Q: %d * %d", rand_A, rand_B);
                    break;
        }
        LCD_print(str);
        
        // Generate the correct letter & the random choices
        srand(TMR5 + TMR2);
        rand_op = (rand() % difficulty) + 1;
        srand(TMR5);
        correct_letter = (rand() % 3) + 1;
        
        while(rand_A == correct_ans || rand_A == rand_B){
            srand(TMR5 + TMR4);
            rand_A = (rand() % 82);
        }
        while(rand_B == correct_ans || rand_B == rand_A){
            srand(TMR5 + TMR4 + TMR2);
            rand_B = (rand() % 82);
        }
        
        switch(correct_letter){
            case 1: sprintf(str, "\na.%d  b.%d  c.%d", correct_ans, rand_A, rand_B);
                    break;
            case 2: sprintf(str, "\na.%d  b.%d  c.%d", rand_A, correct_ans, rand_B);
                    break;
            case 3: sprintf(str, "\na.%d  b.%d  c.%d", rand_A, rand_B, correct_ans);
                    break;
        }
        for(i = 0; i < strlen(str); i++){
            if(str[i] == 'b')
                b_pos = i - 1;
            if(str[i] == 'c')
                c_pos = i - 1;
        }
        LCD_print(str);
        while(state == GAMEMODE2){
            submitted_ans = adcvalue;
            if(adcvalue == 1)
                LCD_send(SETDDRAMADDR | 0x0040);
            else if(adcvalue == 2)
                LCD_send(SETDDRAMADDR | (0x0040 + b_pos));
            else if(adcvalue == 3)
                LCD_send(SETDDRAMADDR | (0x0040 + c_pos));
            if(!PORTAbits.RA0){
                while(!PORTAbits.RA0 && (state == GAMEMODE2));
                break;
            }
        }
        
        if((submitted_ans == correct_letter) && (state == GAMEMODE2))
            score += rand_op_orig;
    }
    
    // #########################################################################
    
    // #########################################################################
    // Game over
    LATBbits.LATB14 = 0;
    LCD_send(CLEARDISPLAY);
    LCD_send(DISPLAY | DISPLAY_ON);
    LCD_print("   GAME OVER");
    sprintf(str, "\n   SCORE: %d", score);
    LCD_print(str);
    GAME_Timer(5);
    while(state == GAMEOVER);
    asm("RESET");
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void){
    if( (!PORTAbits.RA1) || (!PORTAbits.RA2) || (!PORTAbits.RA3) || (!PORTAbits.RA4) ){
        // wait ~15ms for the input to become stable
        delay(DELAY15MS);     // ~15ms delay
        
        if(!PORTAbits.RA1){
            row_pressed = 1;
            keypad_pressed = 1;
        }
        else if(!PORTAbits.RA2){
            row_pressed = 2;
            keypad_pressed = 1;
        }
        else if(!PORTAbits.RA3){
            row_pressed = 3;
            keypad_pressed = 1;
        }
        else if(!PORTAbits.RA4){
            row_pressed = 4;
            keypad_pressed = 1;
        }
        else{    // no input
            row_pressed = -1;
            keypad_pressed = -1;
        }
        
        // determine the column that was pulled down when the interrupt was detected
        if(keypad_pressed == 1){
            if(!PORTBbits.RB7)
                col_pressed = 1;
            else if(!PORTBbits.RB8)
                col_pressed = 2;
            else if(!PORTBbits.RB9)
                col_pressed = 3;
        }
        else
            col_pressed = -1;
    }
    while( ((!PORTAbits.RA1) || (!PORTAbits.RA2) || (!PORTAbits.RA3) || (!PORTAbits.RA4)) && (state != GAMEOVER) );
    IFS1bits.CNIF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    adcvalue = ADC1BUF0;
    if(adcvalue <= 341)
        adcvalue = 1;
    else if(adcvalue > 682)
        adcvalue = 3;
    else
        adcvalue = 2;
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.AD1IF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _T3Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    T2CONbits.TON = 0;      // turn off timer
    TMR2 = 0;
    TMR3 = 0;
    if(state == GAMEMODE1 || state == GAMEMODE2){
        state = GAMEOVER;
        T4CONbits.TON = 0;      // turn off TIMER4
        
        // Disable OCx
        OC2CON1bits.OCM = 0;
        OC1CON1bits.OCM = 0;
        __builtin_write_OSCCONL(OSCCON = 0xBF);     // clear IOLOCK
        RPOR7bits.RP14R = 0;
        __builtin_write_OSCCONL(OSCCON | 0x40);     // set IOLOCK        
    }
    else if(state == GAMEOVER)
        state = START;
    
    __builtin_disi(0x0000);     // enable interrupts
    IFS0bits.T3IF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _T4Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    T4CONbits.TON = 0;      // turn off timer
    
    gametimer_current += TMR4;
    TMR4 = 0;
    if(gametimer_current == 15625){
        freq_counter++;
        gametimer_current = 0;
    }
    if(dutycycle >= 200)
        dc_dir = 1;
    else if(dutycycle <= 0)
        dc_dir = 0;
    
    if(dc_dir)
        dutycycle -= freq_counter;
    else
        dutycycle += freq_counter;
    OC2R = dutycycle;

    __builtin_disi(0x0000);     // enable interrupts
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 1;      // turn on timer
}//*/

void PIC_init(void){
    // Ports Config:
    // LCD: RB5:RB0 -> output
    freq_counter = 1;
    AD1PCFG = 0xFFFF;       // Set all pins as digital
    TRISA = 0x0000;         // Set TRISA to output
    TRISB = 0x0000;         // Set TRISB to output
    
    // Pushbutton: RA0 -> input
    TRISAbits.TRISA0 = 1;       // RA0 is input
    
    // LED: RB14 -> output
    //TRISB = TRISB | 0xBFFF;
    LATBbits.LATB14 = 1;
    
    // Keypad config
    // RA1:RA4 -> row1:row4 (input)
    // RB7:RB9 -> col1:col3 (output)
    TRISA = TRISA | 0x001E;
    
    // Configure internal pull-ups
    // RA1 - CN3 (CNPU1 bit 3)
    // RA2 - CN30 (CNPU2 bit 14)
    // RA3 - CN29 (CNPU2 bit 13)
    // RA4 - CN0 (CNPU1 bit 0)
    CNPU1 = CNPU1 | 0x0009;
    CNPU2 = CNPU2 | 0x6000;
    
    // Enable change notification interrupts
    // and clear IRQ flag
    // RA1 - CN3 (CNEN1 bit 3)
    // RA2 - CN30 (CNEN2 bit 14)
    // RA3 - CN29 (CNEN2 bit 13)
    // RA4 - CN0 (CNEN1 bit 0)
    CNEN1 = CNEN1 | 0x0009;
    CNEN2 = CNEN2 | 0x6000;
    IPC4bits.CNIP = 3;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    LATB = LATB | 0x0380;      // no input from keypad yet
    
    // Potentiometer: RB15 -> input
    ADC_init();
    IFS0bits.AD1IF = 0;     // clear interrupt flag
    IEC0bits.AD1IE = 1;     // enable interrupts
    return;
}

void TMR1_init(void){
    T1CON = 0x0000;         // set T1CON to 0 first
    TMR1 = 0;               // clear tmr1
    T1CONbits.TCS = 0;      // TMR1 clk src is FRC
    T1CONbits.TCKPS = 0;    // FRC is not divided
    T1CONbits.TGATE = 0;    // gate time acc is disabled
    T1CONbits.TSIDL = 0;    // Continue in idle mode
    return;
}

void GAME_Timer(long seconds){
    T2CON = 0x0000;         // clear T2CON
    TMR2 = 0;               // clear TMR2
    TMR3 = 0;               // clear TMR3
    T2CONbits.T32 = 1;      // operate TMR2 as a 32-bit timer
    T2CONbits.TCS = 0;      // Clock source is FRC
    T2CONbits.TCKPS = 3;    // FRC/256 = 15.625kHz
    T2CONbits.TGATE = 0;    // Gated time acc is disabled
    T2CONbits.TSIDL = 0;    // Continue in idle mode
    
    // Set PR3:PR2 value
    long cycles = 15625*seconds;
    PR2 = cycles;
    PR3 = cycles >> 16;
    
    // Configure interrupts
    IPC2bits.T3IP = 6;      // set TMR3 ISR priority
    IFS0bits.T3IF = 0;      // clear interrupt flag
    IEC0bits.T3IE = 1;      // enable interrupts
    
    T2CONbits.TON = 1;      // Turn on timer
    return;
}

void TMR4_init(void){
    T4CON = 0x0000;         // set T4CON to 0 first
    TMR4 = 0;               // clear tmr4
    T4CONbits.TCS = 0;      // TMR4 clk src is FRC
    T4CONbits.TCKPS = 3;    // FRC/256 = 15.625kHz
    T4CONbits.TGATE = 0;    // gate time acc is disabled
    T4CONbits.TSIDL = 0;    // Continue in idle mode
    
    // Set PR4
    PR4 = 125;             // 200ms
    
    // Configure interrupts
    IPC6bits.T4IP = 4;      // set TMR4 ISR priority
    IFS1bits.T4IF = 0;      // clear interrupt flag
    IEC1bits.T4IE = 1;      // enable interrupts
    return;
}

void TMR5_init(void){
    T5CON = 0;
    TMR5 = 0;
    T5CONbits.TCS = 0;      // Clk source is FRC
    T5CONbits.TCKPS = 0;
    T5CONbits.TGATE = 0;
    T5CONbits.TSIDL = 0;
    
    PR5 = 0xFFFF;
    T5CONbits.TON = 1;
    return;
}

void OCx_init(void){
    // Use OC2 as output
    __builtin_write_OSCCONL(OSCCON = 0xBF);     // clear IOLOCK
    RPOR7bits.RP14R = 19;   // Assign OC2 output to RP14
    __builtin_write_OSCCONL(OSCCON | 0x40);     // set IOLOCK
    
    // Clear OCxCON registers
    OC2CON1 = 0;
    OC2CON2 = 0;
    
    // Select timer source as FRC
    OC2CON1bits.OCTSEL = 7;
    
    // Set SYNC source to the module
    OC2CON2bits.SYNCSEL = 0x1F;
    
    // Set OCxR & OCxRS
    OC2RS = 199;        // 20kHz
    OC2R = OC2RS >> 1;  // dutycycle
    
    // Enable modules
    OC2CON1bits.OCM = 6;        // Edge-aligned pwm
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
						// Interrupts at the completion of conversion for
						// each sample/convert sequence
						// Always uses MUX A input mux settings
    
    AD1CON3 = 0x1F3F;   // ADC Clock derived from system clock
						// TAD = 2*TCY, TAD = 135 nSec or 530ksps
    AD1CHS = 0x0009;    // CH0 positive input is AN9
    AD1CSSL = 0x0000;
    return;
}