.include "p24fj64gb002.inc"
.global __reset
    
.bss
i: .space 4

.text
__reset:

    mov #__SP_init, W15
    mov #__SPLIM_init, W0
    mov W0, SPLIM

    clr.b i
  
    avalue = 50000
    bvalue = 40
    mov #bvalue, W0
    mov #avalue, W2
    mov WREG, i
    clr W3
    
    tmrreset:
	clr W0
	clr W1

    loop1:
	cp i	
	
	bra Z, tplus
	loop2: 
	    inc W1, W1
	    cpseq W1, W2
	    goto loop2
	    inc W0, W0
	    goto clrw1
	
    tplus:
	inc W3, W3
	goto tmrreset
	
    clrw1:
	clr W1
	goto loop1
	
    done:
	goto done
    .end
    

