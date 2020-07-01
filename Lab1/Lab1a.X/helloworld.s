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
	clr i
	clr j
	mov #0xFFFC, W0
	mov W0, TRISA
	mov #0x0003, W1

blinky:
	clr PORTA
	call delay
	mov W1, PORTA
	call delay
	bra blinky

delay:
	mov #0x000b, W0
	mov W0, i
	mov #0x2c35, W0
	mov W0, j

loop:
	dec j
	bra nz, loop
	dec i
	bra nz, loop
	return
.end