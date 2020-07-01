.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG2, FNOSC_FRC
config __CONFIG3, SOSCSEL_IO
config __CONFIG1, JTAGEN_OFF & ICS_PGx2

.bss
i: .space 2
j: .space 2
 
.text
__reset:
	mov #__SP_init, W15
	mov #__SPLIM_init, W0
	mov W0, SPLIM
	
	mov #0xFFFF, W0
	mov W0, AD1PCFG		; sets to digital (instead of analog)
	mov #0x0001, W0		; set all A reg to output
	mov W0, TRISA		
	mov #0x0000, W0		; set rb1&2 to output, rb0 to input
	mov W0, TRISB
	clr W0
	clr W1
	
led_init:
    mov #0xFFC0, W0
    mov W0, LATB
    call delay
    call delay
    call delay
    call delay
    mov #0xFFFF, W0
    mov W0, LATB
    call delay
    call delay
    call delay
    call delay
    
led1:
    mov #0xFFFE, W0
    mov W0, LATB
    clr W1
    call delay
    btss PORTA, #0
    call pbwait
    btsc W1, #0
    goto led1
    
led2:
    mov #0xFFFD, W0
    mov W0, LATB
    call delay
    btss PORTA, #0
    call pbwait
    btsc W1, #0
    goto led1
    
led3:
    mov #0xFFFB, W0
    mov W0, LATB
    call delay
    btss PORTA, #0
    call pbwait
    btsc W1, #0
    goto led2
    
led4:
    mov #0xFFF7, W0
    mov W0, LATB
    call delay
    btss PORTA, #0
    call pbwait
    btsc W1, #0
    goto led3
    
led5:
    mov #0xFFEF, W0
    mov W0, LATB
    call delay
    btss PORTA, #0
    call pbwait
    btsc W1, #0
    goto led4
    
led6:
    mov #0xFFDF, W0
    mov W0, LATB
    bset W1, #0
    call delay
    btss PORTA, #0
    call pbwait
    btss W1, #0
    goto led6
    goto led5

delay:
    mov #0x000b, W0
    mov W0, i
    mov #0x2c23, W0
    mov W0, j

loop:
    dec j
    bra nz, loop
    dec i
    bra nz, loop
    return
	
pbwait:
    btg W1, #0
    pbloop:
	btsc PORTA, #0
	return
	goto pbloop
    
.end