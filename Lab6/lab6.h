// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <string.h>
#define DELAY1US 4
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

// flags
static int idle = 1;
static int acked = 0;
static int rx_complete = 0;
static int tx_complete = 0;

// RB<5:0> format: E, RS, D7, D6, D5, D4

void TMR1_init(void);
void delay(unsigned int cyc);       // delay func that uses TMR1
void LCD_init(void);
void LCD_send(unsigned int data);   // send 4bit nibbles to the LCD
void LCD_print(char *str);          // print to the LCD
unsigned int char_code(const char c);// get the code for a given character

void reset_flags();
void wait_idle();
void I2C_init(void);
void I2C_start(void);
void I2C_restart(void);
void I2C_stop(void);
void I2C_ack(void);
void I2C_nack(void);
void I2C_write(int data);
int I2C_read(void);

void TMR1_init(void){
    T1CON = 0x0000;         // set T1CON to 0 first
    TMR1 = 0;               // clear tmr1
    T1CONbits.TCS = 0;      // TMR1 clk src is FRC
    T1CONbits.TCKPS = 0;    // FRC is not divided
    T1CONbits.TGATE = 0;    // gate time acc is disabled
    T1CONbits.TSIDL = 0;    // Continue in idle mode
    return;
}

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

void reset_flags(){
    idle = 1;
    tx_complete = 0;
    rx_complete = 0;
    acked = 0;
}

void wait_idle(){
    while(!idle);
    reset_flags();
}

// Initialize I2C module
void I2C_init(void){	
    // Set values for the ff:
    I2C1CONbits.I2CSIDL = 1;    // stop when idle
    I2C1CONbits.A10M = 0;       // I2C1ADD is 7-bit address
    IFS1bits.MI2C1IF = 0;       // clear interrupt flag
    IEC1bits.MI2C1IE = 1;       // enable interrupt master
    
    I2C1BRG = 9;                // SCL freq = 400kHz at Fcy=4MHz
    I2C1CONbits.I2CEN = 1;      // enable I2C
}

// Send start bit sequence
void I2C_start(void){
    // Set Start Condition Enabled Bit
    wait_idle();
    I2C1CONbits.SEN = 1;
    idle = 0;
}

// Send repeated start bit sequence
void I2C_restart(void){
    // Set Repeated Start Condition Enabled bit
    wait_idle();
    I2C1CONbits.RSEN = 1;
    idle = 0;
}

// Send stop bit sequence
void I2C_stop(void){
    // Set Stop Condition Enabled 
    wait_idle();
    I2C1CONbits.PEN = 1;
    idle = 0;
}

// Send ACK bit sequence
void I2C_ack(void){
    wait_idle();
    idle = 0;
    I2C1CONbits.ACKDT = 0;      // send ACK during acknowledge
    I2C1CONbits.ACKEN = 1;
}

// Send NACK bit sequence
void I2C_nack(void){
    wait_idle();
    idle = 0;
    I2C1CONbits.ACKDT = 1;      // send NACK during acknowledge
    I2C1CONbits.ACKEN = 1;
}

// Transfer one byte sequence
void I2C_write(int data){
    wait_idle();
    idle = 0;
    I2C1TRN = data;
    while(!tx_complete);
    while(!acked);
    reset_flags();
}

// Receive one byte sequence
int I2C_read(void){
    int temp = 0;
    I2C1CONbits.ACKEN = 0;      // bit 4
    I2C1CONbits.RCEN = 0;       // bit 3
    I2C1CONbits.PEN = 0;        // bit 2
    I2C1CONbits.RSEN = 0;       // bit 1
    I2C1CONbits.SEN = 0;        // bit 0
    
    wait_idle();
    idle = 0;
    I2C1CONbits.RCEN = 1;
    
    while(!rx_complete);
    temp = I2C1RCV;
    reset_flags();
    return temp;
}


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

