; 
; This file contains macros designed to be used as a part of the
; smb HL disassembly.
;

; ----------------------------------------------------------------------------

; change to USEDATASEG = 1 to move data blocks to their own segment, rather than mixed with code
; ( leave 0 to match original ROM )

USEDATASEG = 0 

; ----------------------------------------------------------------------------

.autoimport + ; don't declare symbols to be imported
.linecont +   ; allow a '\' to indicate the line is to be continued.

; ----------------------------------------------------------------------------

; macro vram_string:
; ( xx, yy ) = (x,y) location on screen, str = string to display
; This macro will convert the parameters into data acceptable to smb's 
; vram writing routine.

; This works, but wasn't implemented except for a couple tests

.macro vram_string xx,yy,str
    .byte >($2000 + (yy * $20 + xx)), <($2000 + (yy * $20 + xx)), .strlen(str), str
.endmacro

; ----------------------------------------------------------------------------

; mark a block as data ( SEE USEDATASEG above )

.macro dataseg
    .if ::USEDATASEG
        .segment "RODATA"
    .endif
.endmacro

; ----------------------------------------------------------------------------

; output a bit opcode byte

.macro BIT_Skip2
    .byte $2c
.endmacro

; ----------------------------------------------------------------------------

; call this macro once to setup character set encoding used by smb

.macro customchars

    .define CHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    .repeat .strlen(CHARS), I
        .charmap .strat(CHARS, I), I
    .endrepeat
    .undefine CHARS

    .charmap '-', $28
    .charmap 'x', $29
    .charmap '!', $2b
    .charmap ' ', $24
    .charmap '.', $af

.endmacro
