    .include "p30F3013.inc"

    .GLOBAL __T1Interrupt
    .GLOBAL _muestra
;/**@brief ESTA ES LA ISR DEL TIMER
; *
; */	
__T1Interrupt:
	PUSH	W0
	BTG	LATB,	    #LATB8
	MOV	#1,	    W0
	MOV	W0,	   _muestra	
	
	BCLR	IFS0,	    #T1IF	;APAGAR BANDERA DE INTERRUPCION DEL TIMER1
    
	POP	W0
	RETFIE

	
	