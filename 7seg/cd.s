.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG2, FNOSC_FRC

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
	mov #0x0001, W0		; set rb1&2 to output, rb0 to input
	mov W0, TRISB
	clr W0
	
; start 7seg at 0. pressing pb increments the value
; use ra0,1,2 rb1,2,3,15
; a=rb1, b=rb2, c=rb3, d=ra0, e=ra1, f=ra2, g=rb15
	
waitup0:
    mov #0x0007, W0
    mov W0, LATA
    mov #0x000E, W0
    mov W0, LATB
    call delay

waitup1:	; button pushed
	mov #0x0000, W0
	mov W0, LATA
	mov #0x000C, W0
	mov W0, LATB
	call delay

waitup2:	; button pushed
	mov #0x0003, W0
	mov W0, LATA
	mov #0x8006, W0
	mov W0, LATB
	call delay

waitup3:	; button pushed
	mov #0x0001, W0
	mov W0, LATA
	mov #0x800E, W0
	mov W0, LATB
	call delay
	
waitup4:	; button pushed
	mov #0x0004, W0
	mov W0, LATA
	mov #0x800C, W0
	mov W0, LATB
	call delay

waitup5:	; button pushed
	mov #0x0005, W0
	mov W0, LATA
	mov #0x800A, W0
	mov W0, LATB
	call delay

waitup6:	; button pushed
	mov #0x0007, W0
	mov W0, LATA
	mov #0x800A, W0
	mov W0, LATB
	call delay
	
waitup7:	; button pushed
	mov #0x0000, W0
	mov W0, LATA
	mov #0x000E, W0
	mov W0, LATB
	call delay

waitup8:	; button pushed
	mov #0x0007, W0
	mov W0, LATA
	mov #0x800E, W0
	mov W0, LATB
	call delay
	
waitup9:	; button pushed
	mov #0x0005, W0
	mov W0, LATA
	mov #0x800E, W0
	mov W0, LATB
	call delay
	goto waitup0
	
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
.end