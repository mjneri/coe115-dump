.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG2, FNOSC_FRC
config __CONFIG3, SOSCSEL_IO
config __CONFIG1, JTAGEN_OFF
    

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
    mov #0x0000, W0		; set all A reg to output
    mov W0, TRISA		
    mov #0x0000, W0		; set RB[3:0] to output
    mov W0, TRISB
    clr i
    clr j

; D[7:4] = RB[3:0]
; RS = RB[4]
; E = RB[5]
; RB[5:0] = <E, RS, D7, D6, D5, D4>
    
lcd_init:
    call delay15ms
    mov #0x0003, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    call delay4_1ms
    mov #0x0003, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    call delay4_1ms
    mov #0x0003, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    call delay4_1ms
    mov #0x0002, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; function set
    call delay4_1ms
    mov #0x0002, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0008, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; display off
    call delay4_1ms
    mov #0x0000, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0008, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; clear display
    call delay4_1ms
    mov #0x0000, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0001, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; entry mode set
    call delay4_1ms
    mov #0x0000, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0006, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; display on
    call delay4_1ms
    mov #0x0000, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x000F, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    ; initialization complete

; RB[5:0] = <E, RS, D7, D6, D5, D4>
lcd_send4bits:
    ; Write Hello World!
    ; H
    call delay4_1ms
    mov #0x0014, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0018, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; e
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0015, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; l
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x001C, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; l
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x001C, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; o
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x001F, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; space
    call delay4_1ms
    mov #0x0012, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0010, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; w
    call delay4_1ms
    mov #0x0017, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0017, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; o
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x001F, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; r
    call delay4_1ms
    mov #0x0017, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0012, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; l
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x001C, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ;d
    call delay4_1ms
    mov #0x0016, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0014, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; !
    call delay4_1ms
    mov #0x0012, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov #0x0011, W0
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    
    ; finish
    goto done
    
delay100us:
    mov #0x0001, W0
    mov W0, i
    mov #0x0083, W0
    mov W0, j
    goto loop

delay4_1ms:
    mov #0x0001, W0
    mov W0, i
    mov #0x155a, W0
    mov W0, j
    goto loop
    
delay15ms:
    mov #0x0001, W0
    mov W0, i
    mov #0x4e20, W0
    mov W0, j

loop:
    dec j
    bra nz, loop
    dec i
    bra nz, loop
    return
    
done:
    goto done
.end