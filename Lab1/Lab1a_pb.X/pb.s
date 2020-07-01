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
	mov #0xFFFC, W0
	mov W0, TRISA
	mov #0x0001, W0
	mov W0, TRISB
	mov #0x0002, W1

main:
waitdown:
	bset LATA, #0
	bset LATA, #1
	btsc PORTB, #0
	goto waitdown

waitup:	; button pushed
	bclr LATA, #0
	btss PORTB, #0
	goto waitup
	
blinky:
	bset LATA, #0
	bclr LATA, #1
	call delay

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
	goto main
.end