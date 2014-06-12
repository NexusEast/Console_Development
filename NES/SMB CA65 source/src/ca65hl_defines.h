
; BELOW: .defines for recognized flag tests. Format: Flag to test for and default setting. 
; These are not required for the macro code to function

    .define less                C clear
    .define greaterORequal      C set
    .define carry               C set
    .define zero                Z set
    .define equal               Z set
    .define plus                N clear
    .define positive            N clear
    .define minus               N set
    .define negative            N set
    .define bit7                N set
    .define overflow            V set
    .define bit6                V set
    .define bitset              Z clear

    .define greater             G set               ; Use greater and less/equal macros to simulate
    .define lessORequal         G clear

; define symbols for AND, OR, NOT, must use recognized ca65 tokens or valid ident-like strings
; These are required for all functions to work
    

    .define     _HL_MACRO_USER_AND_DEFINITION_       &&     ; '&&' will match '.and'
    .define     _HL_MACRO_USER_OR_DEFINITION_        ||     ; '||' will match '.or'
    .define     _HL_MACRO_USER_NOT_DEFINITION1_      not     
    .define     _HL_MACRO_USER_NOT_DEFINITION2_      !      ; wil match '.not'
    .define     _HL_MACRO_USER_GOTO_DEFINITION_      goto   ; if condition goto label
    
; maximum iny/dey/inx/dex commands created by a +/- in an expression with x or y reg before a warning
    .define _MB_MAXIMUM_ADD_OR_SUB_X_OR_Y_ 6    
    