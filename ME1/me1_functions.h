// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#define DELAY100US 400
#define DELAY4_1MS 16400
#define DELAY15MS 60000

#define CLEARDISPLAY 0x0001
#define RETURNHOME 0x0002

#define ENTRYMODE_INCR 0x0006
#define ENTRYMODE_DECR 0x0004
#define ENTRYMODE_SHL 0x0005
#define ENTRYMODE_SHR 0x0004

#define DISPLAY 0x0008
#define DISPLAY_ON 0x000C
#define DISPLAY_CURSOR 0x000A
#define DISPLAY_BLINKON 0x0009

#define SETDDRAMADDR 0x0080

// RB<5:0> format: E, RS, D7, D6, D5, D4

void delay(unsigned int cyc);       // delay func that uses TMR1
void LCD_init(void);
void LCD_send(unsigned int data);   // send 4bit nibbles to the LCD
void LCD_print(char *str);          // print to the LCD
unsigned int char_code(const char c);// get the code for a given character
int keypad_in(int row, int col);

void delay(unsigned int cyc){
    PR1 = cyc;
    T1CONbits.TON = 1;
    while(TMR1 != PR1);
    T1CONbits.TON = 0;
    TMR1 = 0;
}

void LCD_init(void){
    // RB[5] = E
    // RB[4] = RS
    // RB[3:0] = D[7:4]
    // E, RS, D<7:4>
    
    LATB = LATB & 0xFFC0;
    delay(DELAY15MS);
    LATB = LATB | 0x0003;
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    
    LATB = LATB & 0xFFC0;
    delay(DELAY4_1MS);
    LATB = LATB | 0x0003;
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    
    LATB = LATB & 0xFFC0;
    delay(DELAY4_1MS);
    LATB = LATB | 0x0003;
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    
    LATB = LATB & 0xFFC0;
    delay(DELAY4_1MS);
    LATB = LATB | 0x0002;
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    
    LCD_send(0x0028);       // Function set
    LCD_send(DISPLAY);       // Display off
    LCD_send(CLEARDISPLAY); // Clear display
    LCD_send(ENTRYMODE_INCR);       // Entry mode set
    LCD_send(0x000F);       // Display on
    return;
}

void LCD_send(unsigned int data){
    // data<7:0> contains the data bits D<7:0>
    // data<8> contains RS
    // Upper nibble is sent first.
    
    LATB = LATB & 0xFFC0;
    delay(DELAY4_1MS);
    LATB = LATB | ((data >> 4) & 0x000F);    // take only the upper nibble
    LATBbits.LATB4 = (data >> 8) & 0x0001;  // take RS
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    
    LATB = LATB & 0xFFC0;
    delay(DELAY100US);
    LATB = LATB | (data & 0x000F);
    LATBbits.LATB4 = (data >> 8) & 0x0001;
    LATBbits.LATB5 = 1;
    delay(DELAY100US);
    LATBbits.LATB5 = 0;
    return;
}

void LCD_print(char *str){
    unsigned int len = strlen(str);
    unsigned int i = 0;
    for(i = 0; i < len; i++){
        LCD_send(char_code(str[i]));
    }
    return;
}

unsigned int char_code(const char c){
    // Note; add \n and other special chars
    switch(c){
        case ' ': return 0x0120;
        case '!': return 0x0121;
        case '"': return 0x0122;
        case '#': return 0x0123;
        case '$': return 0x0124;
        case '%': return 0x0125;
        case '&': return 0x0126;
        case '\'': return 0x0127;
        case '(': return 0x0128;
        case ')': return 0x0129;
        case '*': return 0x012a;
        case '+': return 0x012b;
        case ',': return 0x012c;
        case '-': return 0x012d;
        case '.': return 0x012e;
        case '/': return 0x012f;
        case '0': return 0x0130;
        case '1': return 0x0131;
        case '2': return 0x0132;
        case '3': return 0x0133;
        case '4': return 0x0134;
        case '5': return 0x0135;
        case '6': return 0x0136;
        case '7': return 0x0137;
        case '8': return 0x0138;
        case '9': return 0x0139;
        case ':': return 0x013a;
        case ';': return 0x013b;
        case '<': return 0x013c;
        case '=': return 0x013d;
        case '>': return 0x013e;
        case '?': return 0x013f;
        case '@': return 0x0140;
        case 'A': return 0x0141;
        case 'B': return 0x0142;
        case 'C': return 0x0143;
        case 'D': return 0x0144;
        case 'E': return 0x0145;
        case 'F': return 0x0146;
        case 'G': return 0x0147;
        case 'H': return 0x0148;
        case 'I': return 0x0149;
        case 'J': return 0x014a;
        case 'K': return 0x014b;
        case 'L': return 0x014c;
        case 'M': return 0x014d;
        case 'N': return 0x014e;
        case 'O': return 0x014f;
        case 'P': return 0x0150;
        case 'Q': return 0x0151;
        case 'R': return 0x0152;
        case 'S': return 0x0153;
        case 'T': return 0x0154;
        case 'U': return 0x0155;
        case 'V': return 0x0156;
        case 'W': return 0x0157;
        case 'X': return 0x0158;
        case 'Y': return 0x0159;
        case 'Z': return 0x015a;
        case '[': return 0x015b;
        case '¥': return 0x015c;
        case ']': return 0x015d;
        case '^': return 0x015e;
        case '_': return 0x015f;
        case '`': return 0x0160;
        case 'a': return 0x0161;
        case 'b': return 0x0162;
        case 'c': return 0x0163;
        case 'd': return 0x0164;
        case 'e': return 0x0165;
        case 'f': return 0x0166;
        case 'g': return 0x0167;
        case 'h': return 0x0168;
        case 'i': return 0x0169;
        case 'j': return 0x016a;
        case 'k': return 0x016b;
        case 'l': return 0x016c;
        case 'm': return 0x016d;
        case 'n': return 0x016e;
        case 'o': return 0x016f;
        case 'p': return 0x0170;
        case 'q': return 0x0171;
        case 'r': return 0x0172;
        case 's': return 0x0173;
        case 't': return 0x0174;
        case 'u': return 0x0175;
        case 'v': return 0x0176;
        case 'w': return 0x0177;
        case 'x': return 0x0178;
        case 'y': return 0x0179;
        case 'z': return 0x017a;
        case '{': return 0x017b;
        case '|': return 0x017c;
        case '}': return 0x017d;
        case '\n': return 0x00c0;   // set ddram addr
    }
    return 0;
}

int keypad_in(int row, int col){
    // * - 0xA
    // # - 0xB
    switch(row){
        case 1:
            switch(col){
                case 1: return 1;
                case 2: return 2;
                case 3: return 3;
                default: return -1;
            }
            break;
        case 2:
            switch(col){
                case 1: return 4;
                case 2: return 5;
                case 3: return 6;
                default: return -1;
            }
            break;
        case 3:
            switch(col){
                case 1: return 7;
                case 2: return 8;
                case 3: return 9;
                default: return -1;
            }
            break;
        case 4:
            switch(col){
                case 1: return 10;  // *
                case 2: return 0;
                case 3: return 11;  // #
                default: return -1;
            }
            break;
        default: return -1;
    }
}
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

