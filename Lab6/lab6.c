#include <xc.h>
#include "lab6.h"

#pragma config FWDTEN = OFF     // disables WDT
#pragma config JTAGEN = OFF     // JTAG port is disabled
#pragma config POSCMOD = NONE   // Primary osc is disabled
#pragma config OSCIOFNC = ON    // OSCO functions as port I/O
#pragma config FCKSM = CSDCMD   // Clock switching & Clock monitor disabled
#pragma config FNOSC = FRCPLL   // FRC w/ postscaler and PLL module
#pragma config PLL96MHZ = OFF   // 96MHz PLL is enabled by the user, not automatically
#pragma config PLLDIV = NODIV   // Osc input used directly
#pragma config SOSCSEL = IO     // SOSC pins are used for I/O (RA4, RB4)
#pragma config I2C1SEL = PRI    // use primary pins for I2C
#pragma config ICS = PGx2       // enables in-circuit debugging

#define EEPROM_slave_address 0xA0
#define WRITE_ADDR 0x0F
#define READ_ADDR1 0x0B
#define READ_ADDR2 0x40

void EEPROM_write(unsigned int waddr, char wdata[]);
void EEPROM_read(unsigned int raddr);


int main(void){
    // Initialize I2C peripheral
    I2C_init();
    // Initialize LCD
    AD1PCFG = 0xFFFF;
    TRISB = 0x0000;
    TMR1_init();
    LCD_init();
    
    // Write student number to WRITE_ADDR
    EEPROM_write(WRITE_ADDR, "201500244");
    //LCD_print("Done writing");
    
    // Read 16 bytes from READ_ADDR
    EEPROM_read(READ_ADDR2);
    LCD_print("\n");
    EEPROM_read(READ_ADDR1);
    // Display 16 bytes on LCD
    //LCD_print("Done");
    EEPROM_write(WRITE_ADDR, "XXXXXXXXX");
    while(1);
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _MI2C1Interrupt(void){
    __builtin_disi(0x3FFF);     // disable interrupts
    // ACK received
    if (!I2C1STATbits.ACKSTAT){  
        acked = 1;
    }
    
    // RX completed
    if (I2C1STATbits.RBF){      
        rx_complete = 1;
    }
    
    // TX completed
    if (!I2C1STATbits.TRSTAT && !I2C1STATbits.TBF){     
        tx_complete = 1;
    }

    // bus is idle or no write collision
    if (!I2C1STATbits.IWCOL || !I2C1STATbits.BCL){    
        idle = 1;
    }
    
    __builtin_disi(0x0000);     // enable interrupts
    IFS1bits.MI2C1IF = 0;
}

// EEPROM write sequence
void EEPROM_write(unsigned int waddr, char wdata[]){
    // Start I2C communication
    I2C_start();

    // Send I2C EEPROM slave address with write command, wait if device is not available
    I2C_write(EEPROM_slave_address + 0);

    // Send address, high byte then low byte
    I2C_write(waddr>>8);
    I2C_write(waddr);

    // Send data
    unsigned int len = strlen(wdata);
    unsigned int i = 0;
    for(i = 0; i < len; i++){
        I2C_write(wdata[i]);
    }
    //I2C_write(0);

    // Stop I2C communication
    I2C_stop();
}

// EEPROM read sequence
void EEPROM_read(unsigned int raddr){
    //unsigned char Byte = 0;

    // Start I2C communication
    I2C_start();

    // Send I2C EEPROM slave address with write command, wait if device is not available
    I2C_write(EEPROM_slave_address + 0);

    // Send address, high byte then low byte
    I2C_write(raddr>>8);
    I2C_write(raddr);

    // Send repeated start
    I2C_restart();

    // Send I2C EEPROM slave address with read command
    I2C_write(EEPROM_slave_address + 1);

    // Store data from buffer
    char lcdString[16];
    int i = 0;
    for(i = 0; i < 17; i++){
        lcdString[i] = I2C_read();
        // send ACK, but not on last byte
        if (i < 16){
            I2C_ack();
        }
    }
    lcdString[i - 1] = 0;

    // Send NACK
    I2C_nack();

    // Stop I2C communication
    I2C_stop();
    LCD_print(lcdString);

    //return Byte;

}