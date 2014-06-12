.segment "HEADER"
	
	.byte	"NES", $1A	; iNES header identifier
	.byte	2		; 2x 16KB PRG code
	.byte	1		; 1x  8KB CHR data
	.byte	$02, $00	; mapper 0, vertical mirroring

;;;;;;;;;;;;;;;

;;; "nes" linker config requires a STARTUP section, even if it's empty
.segment "STARTUP"

.segment "CODE"

	;; Execution starts here after power up or reset.
reset:
	sei			; disable IRQs
	cld			; disable decimal mode
	ldx	#$40		; disable APU frame IRQ
	stx	$4017		;  .
	ldx	#$ff		; Set up stack
	txs				;  .
	inx				; now X = 0
	stx	$2000		; disable NMI
	stx	$2001		; disable rendering
	stx	$4010		; disable DMC IRQs

	;; First wait for vblank to make sure PPU is ready.
vblankwait1:
	bit	$2002
	bpl	vblankwait1

	;; Initialize RAM $0000 - $07ff (2 KB). 
clear_memory:
	lda	#$00
	sta	$0000, x
	sta	$0100, x
	sta	$0300, x
	sta	$0400, x
	sta	$0500, x
	sta	$0600, x
	sta	$0700, x
	lda	#$fe		; sprites are located at $0200 - $02ff
	sta	$0200, x	; $fe moves all sprites off screen
	inx
	bne	clear_memory
vblankwait2:
	bit	$2002
	bpl	vblankwait2
	;; The color palettes are located in VRAM at $3f00 - $3f1f
	;; Load from palette in ROM.
load_palettes:
	lda	$2002		; read PPU status to reset the high/low latch
	lda	#$3f		; write the high byte of $3f00
	sta	$2006		;  .
	lda	#$00		; write the low byte of $3f00
	sta	$2006		;  .
	ldx	#$00		; x = 0
@loop:
	lda	palette, x	; load byte from ROM address (palette + x)
	sta	$2007		; write to PPU
	inx			; x = x + 1
	cpx	#$20		; x == $20?
	bne	@loop		; No, jump to @loop, yes, fall through

	;; Sprites are located in RAM at $0200 - $02ff. We are only
	;; using 4 of the 64 sprites, $0200 - $020f. Load from sprites
	;; in ROM.


load_sprites:

	ldx	#$00		; x = 0
@loop:
	lda	sprites, x	; load byte from ROM address (sprites + x)
	sta	$0200, x	; store into RAM address ($0200 + x)
	inx				; x = x + 1
	cpx	#$10		; x == $10?
	bne	@loop

	ldx #$50 ; x position
	ldy #$50 ; y position 
	sprite_index .set $0200









; stx $0200,a
	
; 	pha				;STACK:[A
; 	txa
; 	pha				;STACK:[AX
; 	tya
; 	pha				;STACK:[AXY

; 	; set y pos to sprite:
; 	; sta $0200,a

; 	iny
; 	iny
; 	iny
; 	iny
; 	inx
; 	inx
; 	inx
; 	inx

; 	cpx #$50
; 	bne @setposition

 


	lda	#%10000000	; enable NMI, sprites from Pattern Table 0
	sta	$2000
	lda	#%00010000	; enable sprites
	sta	$2001


 

forever:
	bit $2002 
	bpl forever


  	speed .set $2
	txa
	clc
	adc #speed
	tax

	sprite_index .set $0200
	jsr setposition
	

	jmp	forever
 

nmi:  
  


	lda #$0 
	sta	$2003  
	lda  #$02
	sta $4014 

 
	rti			; return from interrupt

setposition: 
	tya 
	sta 	sprite_index			;1/4 sprite y
	txa 
	blah 	.set sprite_index+3 	;1/4 sprite x
	sta 	blah

	tya
	blah	.set sprite_index+4 	;2/4 sprite y
	sta 	blah

	txa
	blah	.set sprite_index+7 	;2/4 sprite x
	clc
	adc 	#$8
	sta 	blah  

	tya								;3/4 sprite y
	blah	.set sprite_index+8 
	clc
	adc 	#$8
	sta 	blah

	txa								;3/4 sprite x
	blah	.set sprite_index+$B  
	sta 	blah

	tya								;4/4 sprite y
	blah	.set sprite_index+$c  
	clc
	adc 	#$8
	sta 	blah
	
	txa								;4/4 sprite x
	blah	.set sprite_index+$f  
	clc
	adc 	#$8
	sta 	blah

	rts
 
 


palette:
	;; Background palette
	.byte	$0F,$31,$32,$33
	.byte	$0F,$35,$36,$37
	.byte	$0F,$39,$3A,$3B
	.byte	$0F,$3D,$3E,$0F
	;; Sprite palette
	.byte 	$0F,$16,$27,$18
	.byte	$0F,$02,$38,$3C
	.byte	$0F,$1C,$15,$14
	.byte	$0F,$02,$38,$3C

sprites:
	;;		vert tile attr horiz
	.byte	$80, $32, $00, $80 ; sprite 0
	.byte	$80, $33, $00, $88 ; sprite 1
	.byte	$88, $34, $00, $80 ; sprite 2
	.byte	$88, $35, $00, $88 ; sprite 3

;;;;;;;;;;;;;;  
  
.segment "VECTORS"

	.word	0, 0, 0		; Unused, but needed to advance PC to $fffa.
	
	;; When an NMI happens (once per frame if enabled), it will
	;; jump to the label nmi.
	.word	nmi
	;; When the processor first turns on or is reset, it will jump to the
	;; label reset.
	.word	reset
	;; External interrupt IRQ is not used in this tutorial 
	.word	0
  
;;;;;;;;;;;;;;  
  
.segment "CHARS"
	;; CHR-ROM, 8 KB
	.incbin	"mario.chr"	; includes 8KB graphics from SMB1
	; .incbin	"sprite.chr"  
 