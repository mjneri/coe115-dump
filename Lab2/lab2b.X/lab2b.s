.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG1, JTAGEN_OFF
config __CONFIG2, FNOSC_FRC
config __CONFIG3, SOSCSEL_IO
    
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
    mov #0x0001, W0		; set RA0 to input
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
    mov #0x0002, W0
    mov #0x0008, W1
    call lcd_send4bits
    
    ; display off
    mov #0x0000, W0
    mov #0x0008, W1
    call lcd_send4bits
    
    ; clear display
    mov #0x0000, W0
    mov #0x0001, W1
    call lcd_send4bits
    
    ; entry mode set
    mov #0x0000, W0
    mov #0x0006, W1
    call lcd_send4bits
    
    ; display on
    mov #0x0000, W0
    mov #0x000C, W1
    call lcd_send4bits
    ; initialization complete

; RB[5:0] = <E, RS, D7, D6, D5, D4>
view1: ; Name, student number
    ; clear display
    mov #0x0000, W0
    mov #0x0001, W1
    call lcd_send4bits
    
    ; M
    mov #0x0014, W0
    mov #0x001D, W1
    call lcd_send4bits
    
    ; space
    mov #0x0012, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; N
    mov #0x0014, W0
    mov #0x001E, W1
    call lcd_send4bits
    
    ; e
    mov #0x0016, W0
    mov #0x0015, W1
    call lcd_send4bits
    
    ; r
    mov #0x0017, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; i
    mov #0x0016, W0
    mov #0x0019, W1
    call lcd_send4bits
    
    ; \n
    mov #0x000C, W0
    mov #0x0000, W1
    call lcd_send4bits
    
    ; 2
    mov #0x0013, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 1
    mov #0x0013, W0
    mov #0x0011, W1
    call lcd_send4bits
    
    ; 5
    mov #0x0013, W0
    mov #0x0015, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 2
    mov #0x0013, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; 4
    mov #0x0013, W0
    mov #0x0014, W1
    call lcd_send4bits
    
    ; 4
    mov #0x0013, W0
    mov #0x0014, W1
    call lcd_send4bits
    
    ; set ddram
    mov #0x0009, W0
    mov #0x0008, W1
    call lcd_send4bits
    
    ; M
    mov #0x0014, W0
    mov #0x001D, W1
    call lcd_send4bits
    
    ; space
    mov #0x0012, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; N
    mov #0x0014, W0
    mov #0x001E, W1
    call lcd_send4bits
    
    ; e
    mov #0x0016, W0
    mov #0x0015, W1
    call lcd_send4bits
    
    ; r
    mov #0x0017, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; i
    mov #0x0016, W0
    mov #0x0019, W1
    call lcd_send4bits
    
    ; \n
    mov #0x000D, W0
    mov #0x0008, W1
    call lcd_send4bits
    
    ; 2
    mov #0x0013, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 1
    mov #0x0013, W0
    mov #0x0011, W1
    call lcd_send4bits
    
    ; 5
    mov #0x0013, W0
    mov #0x0015, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 2
    mov #0x0013, W0
    mov #0x0012, W1
    call lcd_send4bits
    
    ; 4
    mov #0x0013, W0
    mov #0x0014, W1
    call lcd_send4bits
    
    ; 4
    mov #0x0013, W0
    mov #0x0014, W1
    call lcd_send4bits
    
    ; shift display
    mov #0x000F, W3
    call delay500ms
view1_loop:
    btss PORTA, #0
    goto view2
    call lcd_displayshift
    dec W3, W3
    bra nz, view1_loop
    goto view1

view2:
    ; clear display
    mov #0x0000, W0
    mov #0x0001, W1
    call lcd_send4bits
    
     ; 1
    mov #0x0013, W0
    mov #0x0011, W1
    call lcd_send4bits
    
    ; 6
    mov #0x0013, W0
    mov #0x0016, W1
    call lcd_send4bits
    
    ; 5
    mov #0x0013, W0
    mov #0x0015, W1
    call lcd_send4bits
    
    ; space
    mov #0x0012, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; c
    mov #0x0016, W0
    mov #0x0013, W1
    call lcd_send4bits
    
    ; m
    mov #0x0016, W0
    mov #0x001d, W1
    call lcd_send4bits
    
    ; \n
    mov #0x000C, W0
    mov #0x0000, W1
    call lcd_send4bits
    
    ; space
    mov #0x0012, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; 8
    mov #0x0013, W0
    mov #0x0018, W1
    call lcd_send4bits
    
    ; 0
    mov #0x0013, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; space
    mov #0x0012, W0
    mov #0x0010, W1
    call lcd_send4bits
    
    ; k
    mov #0x0016, W0
    mov #0x001b, W1
    call lcd_send4bits
    
    ; g
    mov #0x0016, W0
    mov #0x0017, W1
    call lcd_send4bits
    
    ; shift display
    mov #0x000A, W3
    call delay500ms
view2_loop:
    btsc PORTA, #0
    goto view2_waitdown
    call lcd_displayshift
    dec W3, W3
    bra nz, view2_loop
    mov #0x000A, W3
view2_loop_L:
    btsc PORTA, #0
    goto view2_waitdown
    call lcd_displayshift_L
    dec W3, W3
    bra nz, view2_loop_L
    mov #0x000A, W3
    goto view2_loop
    
    
view2_waitdown:
    btsc PORTA, #0
    goto view2_waitdown

view3:
    call lcd_cleardisplay
    call lcd_mcdo
    call lcd_newline
    call lcd_jollibee
    btsc PORTA, #0
    goto view3_waitdown
    call delay500ms
    call delay500ms
    
    call lcd_cleardisplay
    call lcd_jollibee
    call lcd_newline
    call lcd_chowking
    btsc PORTA, #0
    goto view3_waitdown
    call delay500ms
    call delay500ms
    
    call lcd_cleardisplay
    call lcd_chowking
    call lcd_newline
    call lcd_kfc
    btsc PORTA, #0
    goto view3_waitdown
    call delay500ms
    call delay500ms
    
    call lcd_cleardisplay
    call lcd_kfc
    call lcd_newline
    call lcd_burgerking
    btsc PORTA, #0
    goto view3_waitdown
    call delay500ms
    call delay500ms
    
    ;call lcd_cleardisplay
    ;call lcd_chowking
    ;call lcd_newline
    ;call lcd_kfc
    ;btsc PORTA, #0
    ;goto view3_waitdown
    ;call delay500ms
    ;call delay500ms
    
    ;call lcd_cleardisplay
    ;call lcd_jollibee
    ;call lcd_newline
    ;call lcd_chowking
    ;btsc PORTA, #0
    ;goto view3_waitdown
    ;call delay500ms
    ;call delay500ms
    
    call lcd_cleardisplay
    call lcd_burgerking
    call lcd_newline
    call lcd_mcdo
    btsc PORTA, #0
    goto view3_waitdown
    call delay500ms
    call delay500ms
    goto view3
    
view3_waitdown: ; 10s delay
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    call delay500ms
    goto view1
    
    ; finish
    goto done

lcd_cleardisplay:
    ; clear display
    mov #0x0000, W0
    mov #0x0001, W1
    call lcd_send4bits
    return
    
lcd_displayshift:
    mov #0x0001, W0
    mov #0x000C, W1
    call lcd_send4bits
    call delay500ms
    return
    
lcd_displayshift_L:
    mov #0x0001, W0
    mov #0x0008, W1
    call lcd_send4bits
    call delay500ms
    return
    
delay500ms:
    mov #0x000b, W2
    mov W2, i
    mov #0x2c23, W2
    mov W2, j
    goto loop
    
delay100us:
    mov #0x0001, W2
    mov W2, i
    mov #0x0083, W2
    mov W2, j
    goto loop

delay4_1ms:
    mov #0x0001, W2
    mov W2, i
    mov #0x155a, W2
    mov W2, j
    goto loop
    
delay15ms:
    mov #0x0001, W2
    mov W2, i
    mov #0x4e20, W2
    mov W2, j

loop:
    dec j
    bra nz, loop
    dec i
    bra nz, loop
    return
    
lcd_send4bits:
    call delay4_1ms
    mov W0, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    call delay100us
    mov W1, LATB
    bset LATB, #5
    call delay100us
    bclr LATB, #5
    return
    
lcd_newline:
    mov #0x000C, W0
    mov #0x0000, W1
    call lcd_send4bits
    return
    
lcd_mcdo:
    mov #0x0014, W0;M
    mov #0x001d, W1
    call lcd_send4bits
    mov #0x0016, W0;c
    mov #0x0013, W1
    call lcd_send4bits
    mov #0x0016, W0;d
    mov #0x0014, W1
    call lcd_send4bits
    mov #0x0016, W0;o
    mov #0x001f, W1
    call lcd_send4bits
    mov #0x0016, W0;n
    mov #0x001e, W1
    call lcd_send4bits
    mov #0x0016, W0;a
    mov #0x0011, W1
    call lcd_send4bits
    mov #0x0016, W0;l
    mov #0x001c, W1
    call lcd_send4bits
    mov #0x0016, W0;d
    mov #0x0014, W1
    call lcd_send4bits
    mov #0x0012, W0;'
    mov #0x0017, W1
    call lcd_send4bits
    mov #0x0017, W0;s
    mov #0x0013, W1
    call lcd_send4bits
    return
    
lcd_jollibee:
    mov #0x0014, W0;J
    mov #0x001a, W1
    call lcd_send4bits
    mov #0x0016, W0;o
    mov #0x001f, W1
    call lcd_send4bits
    mov #0x0016, W0;l
    mov #0x001c, W1
    call lcd_send4bits
    mov #0x0016, W0;l
    mov #0x001c, W1
    call lcd_send4bits
    mov #0x0016, W0;i
    mov #0x0019, W1
    call lcd_send4bits
    mov #0x0016, W0;b
    mov #0x0012, W1
    call lcd_send4bits
    mov #0x0016, W0;e
    mov #0x0015, W1
    call lcd_send4bits
    mov #0x0016, W0;e
    mov #0x0015, W1
    call lcd_send4bits
    return
    
lcd_chowking:
    mov #0x0014, W0;C
    mov #0x0013, W1
    call lcd_send4bits
    mov #0x0016, W0;h
    mov #0x0018, W1
    call lcd_send4bits
    mov #0x0016, W0;o
    mov #0x001f, W1
    call lcd_send4bits
    mov #0x0017, W0;w
    mov #0x0017, W1
    call lcd_send4bits
    mov #0x0012, W0;space
    mov #0x0010, W1
    call lcd_send4bits
    mov #0x0014, W0;K
    mov #0x001b, W1
    call lcd_send4bits
    mov #0x0016, W0;i
    mov #0x0019, W1
    call lcd_send4bits
    mov #0x0016, W0;n
    mov #0x001e, W1
    call lcd_send4bits
    mov #0x0016, W0;g
    mov #0x0017, W1
    call lcd_send4bits
    return
    
lcd_kfc:
    mov #0x0014, W0;K
    mov #0x001b, W1
    call lcd_send4bits
    mov #0x0014, W0;F
    mov #0x0016, W1
    call lcd_send4bits
    mov #0x0014, W0;C
    mov #0x0013, W1
    call lcd_send4bits
    return
    
lcd_burgerking:
    mov #0x0014, W0;B
    mov #0x0012, W1
    call lcd_send4bits
    mov #0x0017, W0;u
    mov #0x0015, W1
    call lcd_send4bits
    mov #0x0017, W0;r
    mov #0x0012, W1
    call lcd_send4bits
    mov #0x0016, W0;g
    mov #0x0017, W1
    call lcd_send4bits
    mov #0x0016, W0;e
    mov #0x0015, W1
    call lcd_send4bits
    mov #0x0017, W0;r
    mov #0x0012, W1
    call lcd_send4bits
    mov #0x0012, W0;space
    mov #0x0010, W1
    call lcd_send4bits
    mov #0x0014, W0;K
    mov #0x001b, W1
    call lcd_send4bits
    mov #0x0016, W0;i
    mov #0x0019, W1
    call lcd_send4bits
    mov #0x0016, W0;n
    mov #0x001e, W1
    call lcd_send4bits
    mov #0x0016, W0;g
    mov #0x0017, W1
    call lcd_send4bits
    return
    
done:
    goto done
.end