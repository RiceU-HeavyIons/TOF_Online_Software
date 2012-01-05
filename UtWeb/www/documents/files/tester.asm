; MRPC Connection Tester Firmware
; $Id: tester.asm,v 1.2 2007/01/30 16:03:05 koheik Exp $
#include <p16F913.inc>

; thresholds
TL	equ	0x66		; 2V (upper 8bit of 10bit value)	; nominal 'low' voltage is about 1.6V
TH	equ	0xCC		; 4V (upper 8bit of 10bit value)


; PORTA
EN1			equ		0
EN2			equ		1
EN3			equ		2
EN4			equ		3
INP			equ		5
; PORTC
GREEN		equ		0
RED			equ		1	
; Delay
ODLY		equ		0x03		; delay = ODLY x 3 x 256us. delay ~ 10ms when ODLY = 13.
								; number of all configulation is C(24, 2) = 276
								; total time ~ 276 x 10ms ~ 3sec
DEBUG		equ		0x01		; extra delay for debug (normal = 1)

	__config(_MCLRE_ON & _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _CP_OFF & _IESO_OFF)

	cblock 0x20
fDLY1
fDLY2
fDLY3
fCH1
fCH2
fWRK
;fERR
	endc

	org 0
	goto	INIT
	nop
	nop
	nop
	nop
	nop
	nop
	goto	$
INIT
	bcf		STATUS, RP0			; bank 0
	bcf		STATUS, RP1
	clrf	PORTA
	clrf	PORTB
	clrf	PORTC
	movlw	B'00010001'			; left just, AN4
	movwf	ADCON0

	bsf		STATUS, RP0			; bank 1
	movlw	0x07
	movwf	CMCON0				; turn off comparator
	movlw	0x10				; RA4 input
	movwf	ANSEL				;
	movlw	0xF0
	movwf	TRISA				; RA<3:0> output
;
	clrf	TRISB				; output for addresses
	clrf	TRISC				; output for LEDs

	bcf		STATUS, RP0			; bank 2
	bsf		STATUS, RP1			;
	clrf	LCDCON				; disable LCD module
	movlw	B'00100000'			; Fosc/32
	movwf	ADCON1

	bcf		STATUS, RP1			; bank 0

MAIN
;	clrf	fERR				; clear error counter
	movlw	D'23'
	movwf	fCH1				; init outer loop index i = 23 [23, 0)
LOOP
	movfw	fCH1				; init inner loop index j = i-1 [i-1, 0]
	movwf	fCH2
	decf	fCH2, f

LOOP_1
	call	SETCH
	call	DLY
	bsf		ADCON0, 1			; go
	btfsc	ADCON0, 1
	goto	$-1					; wait conversion

	movfw	fCH2				; if j != 4n or 4n + 1-> open
	andlw	0x02
	btfss	STATUS, Z
	goto	OPEN

	movfw	fCH2				; if j == i - 2 -> close
	subwf	fCH1, w
	xorlw	0x02
	btfss	STATUS, Z
	goto	OPEN

CLOSE							; expect close circuit
	movfw	ADRESH
	sublw	TL
	btfss	STATUS, C			; when it's close, input goes low -> borrow
	goto	ERR
;	incf	fERR, f
	goto	LOOP_2

OPEN							; expect open circuit	
	movfw	ADRESH
	sublw	TH
	btfsc	STATUS, C			; when it's open, input goes high -> no borrow
	goto	ERR
;	incf	fERR, f

LOOP_2
	decf	fCH2, f
	incfsz	fCH2, w
	goto	LOOP_1				; do inner loop again if j >= 0

	decfsz	fCH1, f
	goto	LOOP				; do outer loop again if i > 0

;	bcf		PORTC, GREEN		; clear LEDs first
;	bcf		PORTC, RED
;	movfw	fERR				; ceck the error coouter
;	clrf	fWRK
;	xorlw	0x00
;	btfss	STATUS, Z
;	bsf		fWRK, RED
;	btfsc	STATUS, Z
;	bsf		fWRK, GREEN
;	movfw	fWRK
;	movwf	PORTC

	bsf		PORTC, GREEN
	bcf		PORTC, RED
	goto	$					; stay here

ERR
	bcf		PORTC, GREEN
	bsf		PORTC, RED
	goto	$					; stay here


SETCH						; set channel: parameter: fCH1 and fCH2
	movfw	fCH1			; (fCH1 & 0x0F) | (fCH2 << 4)
	andlw	0x0F			;  -> ADDRESS (PORTB)
	movwf	fWRK
	swapf	fCH2, w
	andlw	0xF0
	iorwf	fWRK, w
	movwf	PORTB
	
	clrf	fWRK
	btfss	fCH1, 4			; setup one half
	bsf		fWRK, EN1
	btfsc	fCH1, 4
	bsf		fWRK, EN2

	btfss	fCH2, 4			; setup the other
	bsf		fWRK, EN3
	btfsc	fCH2, 4
	bsf		fWRK, EN4
	movfw	fWRK
	movwf	PORTA
	return

DLY							; delay: parameter: OLDY and DEBUG
	clrf	fDLY1			; inner loop index
	movlw	ODLY
	movwf	fDLY2			; outer loop index
	movlw	DEBUG
	movwf	fDLY3			; extra loop index for debug
DLY_LOOP
	decfsz	fDLY1, f
	goto 	DLY_LOOP
	decfsz	fDLY2, f
	goto 	DLY_LOOP
	decfsz	fDLY3, f
	goto	DLY_LOOP
	return
	
	end
