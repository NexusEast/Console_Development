; CA65 HLA macro pack v10s

; Copyright Movax big.jay.tee@gmail.com
; You are free: to Share to copy, distribute and transmit the work
; to Remix to adapt the work
; to make commercial use of the work
; Under the following conditions:

; Attribution You must attribute the work to big.jay.tee@gmail.com in documentation or with any other credits.
;
; And you must read and be aware of the implications here: http://creativecommons.org/licenses/by/3.0/us/
;
;---------------------------------------------------------------------------------------------------------------------------------------------------
;
; Adds IF-ELSE-ENDIF, DO-WHILE, WHILE-DO-ENDWHILE,REPEAT-UNTIL
; As well as the ability to use a 'function' macro to call other code in a way compatible with the above
; Included is an expression evaluation for unsigned (comp) and signed (scomp) comparisons. Note that any comparison using greater (>) or less (<) cannot also include a
; low or high byte operator (< or > ) unless it is enclosed in brackets.
;
; Please send any bugs to the above email or message Movax12 at forums.nesdev.com
; As well there are two macros named mb, mw for (m)ove (b)yte (m)ove (w)ord that use an := assingment operator
;
; Expression to test represents the flags of the 6502
; All expressions evaluate into a singe branch instruction with the exception of 'greater' and 'not greater'/'lessORequal'
; which is handled with two small macros
;
; Only simple flag name is evaluated (ie C,Z,N,V) along with set/clear after
; a second 'clear' will negate the flag check, a leading 'not' will negate the entire expression and is applied last
;
; FORMAT
; In order: Optional: 'not', Required: flag set/clear, Optional: additional 'clear' or 'set'
; Some sytax and error checking.
;
; For the latest update and more documentation: https://www.assembla.com/spaces/ca65hl/wiki

; ===============================================================================================================    
; Edit this file for optinal settings: 
.include "ca65hl_defines.h"    
; ===============================================================================================================    

    ; branch array
    ; instruction 0 means flag clear, 1 means set

    .define ___CPU_FLAG_COMMAND_1_1  bcs
    .define ___CPU_FLAG_COMMAND_1_2  beq
    .define ___CPU_FLAG_COMMAND_1_3  bmi
    .define ___CPU_FLAG_COMMAND_1_4  bvs
    .define ___CPU_FLAG_COMMAND_0_1  bcc
    .define ___CPU_FLAG_COMMAND_0_2  bne
    .define ___CPU_FLAG_COMMAND_0_3  bpl
    .define ___CPU_FLAG_COMMAND_0_4  bvc

    .define ___CPU_FLAG_COMMAND_1_5 branch_if_greater
    .define ___CPU_FLAG_COMMAND_0_5 branch_if_less_or_equal

.scope ___HL

    _IF_ENDIF__IF_label     .set    0 ; total number of IFs used
    _IF_ENDIF__END_label    .set    0
    _IF_ENDIF__flagnum      .set    0
    _IF_ENDIF__flagset      .set    0


    _DO_WHILE__DO_label     .set    0
    _DO_WHILE__WHILE_label  .set    0
    _DO_WHILE__flagnum      .set    0
    _DO_WHILE__flagset      .set    0


    _WHILE_ENDWHILE__WHILE_label    .set    0
    _WHILE_ENDWHILE__END_label      .set    0
    _WHILE_ENDWHILE__flagnum        .set    0
    _WHILE_ENDWHILE__flagset        .set    0


    ; track loop type we are currently inside (for 'break' statement)
    _CURRENT___LOOP_TYPE_       .set    0

    __MACRO_FUNCTION_COMPLETE_  .set    0 ; signal back to the "Process_Expression" that function is done sucessfully
    _FOUND_IDENTIFIER_              .set    0

    ; temp variables for parsing strings / tokens
    _AND_POS_       .set    0
    _OR_POS_        .set    0
    _AND_POS_copy   .set    0
    _OR_POS_copy    .set    0
    all_or_before_and .set  0


    _LONG_JUMP_     .set    0                   ; options for generating long jumps
    _BRANCH_LENGTH_CHECKING_    .set    1

    ; option to save reg a in .macro scomp:
    ; This will generate extra code: an additional pha, pla and asl on top of regular code and use carry to branch.
    __KEEP_REG_A_ .set 1

.endscope

.enum ___CURRENT_LOOP_
    _NO_LOOP_
    _DO_WHILE_
    _WHILE_DO_
.endenum

;----------------------------------------------------------------------------

.macro branch_if_greater label
.local over
    beq over
    bcs label
    over:
.endmacro

.macro branch_if_less_or_equal label
    bcc label
    beq label
.endmacro

; Set options

.macro set_long_branch option, messages

    .if .xmatch(option, +)
        ___HL::_LONG_JUMP_ .set 1
    .elseif .xmatch(option,-)
        ___HL::_LONG_JUMP_ .set 0
    .else
        .out "Unknown branch setting, branches are set to LONG."
        ___HL::_LONG_JUMP_ .set 1
    .endif
    
    ; disabled for now
    
        ___HL::_BRANCH_LENGTH_CHECKING_.set 0


.endmacro


; .macro set_long_branch option, messages
;     .if .xmatch(option, +)
;         ___HL::_LONG_JUMP_ .set 1
;     .elseif .xmatch(option,-)
;         ___HL::_LONG_JUMP_ .set 0
;     .else
;         .out "Unknown branch setting, branches are set to LONG."
;         ___HL::_LONG_JUMP_ .set 1
;     .endif
; 
;     .if .xmatch(messages, +)
;         ___HL::_BRANCH_LENGTH_CHECKING_ .set 1
;     .elseif .xmatch(messages,-) .or .xmatch(option,-)
;         ___HL::_BRANCH_LENGTH_CHECKING_.set 0
;     .else
;         .out "Unknown branch message setting, set to OFF."
;         ___HL::_BRANCH_LENGTH_CHECKING_.set 0
;     .endif
; .endmacro
; 


; Common macro 'sub' routines

.macro checkbrackets express ; this can't error check too many closing brackets, it will match first properly closed bracket  that matches the starting bracket

    ___HL::_OR_POS_ .set 0 ; use as temp
    ___HL::_AND_POS_ .set 0
    findposition {express}, (, ___HL::_OR_POS_

    .if ___HL::_OR_POS_ = -1 ; if bracket  is on very left side, look for matching close, otherwise ignore
        ___HL::_OR_POS_ .set 0
        matchbrackets {express}, ___HL::_OR_POS_, (,)
        .if ___HL::_OR_POS_ = .tcount({express})  - 1 ; if bracket is on the very outside:
            ___HL::_AND_POS_ .set -1
         .else
            .if ___HL::_OR_POS_ = 0 ; not found
                .error " Brackets do not match."
            .endif
        .endif

    .endif

.endmacro

; Find the position and exact value of a token 'tok' in 'exp' and change the value of 'position' whilst skipping any tokens
; inside matching openskip and closeskip - nested
; Does not match very 1st character at pos 0, beacuse 0 means not found
; will return -1 if character found at pos 0 (this is better generally for using with .mid() )

.macro findposition exp, tok, position, openskip, closeskip, altopenskip, altcloseskip

    .if .xmatch({.mid(position,1,{exp})},{tok})

        .if position = 0
            position .set  -1
        .endif
        ; .out .sprintf( "found: %d", position)
        .exitmacro
        ; done
    .else

         position .set position + 1 ; increase index

        .if (position > .tcount({exp}))
            ; no more tokens, not found, set pos to 0
            position .set 0
            .exitmacro
        .endif

        ; no match yet:
        .ifnblank {openskip} ; we already increased the index, so check if we skipped an open bracket then expand a new macro to find close bracket honoring other brackets
            .if .xmatch({.mid(position-1,1,{exp})},{openskip})
        ;          .out .sprintf( "found ( at %d", position - 1)
                findposition {exp}, {closeskip}, position , openskip, closeskip
        ;         .out .sprintf( "found ) at %d", position)
                position .set position + 1 ; skip the close bracket we just found
            .endif
        .endif

        .if .not (.blank( {altopenskip}))
            .if .xmatch({.mid(position-1,1,{exp})},{altopenskip})
                 ; .out .sprintf( "found [ at %d", position - 1)
                findposition {exp}, {altcloseskip}, position
                 ; .out .sprintf( "found ] at %d", position)
                position .set position + 1 ; skip the close bracket we just found
            .endif
        .endif

        findposition {exp}, {tok}, position, openskip, closeskip, altopenskip, altcloseskip

    .endif
.endmacro


; find a token that is a string/ident type, note = -1 if not found

.macro findidentstring exp, position, openskip, closeskip, altopenskip, altcloseskip

    .if .match({.mid(position,1,{exp})}, arandomstring)


        ; .out .sprintf( "found: %d", position)
        .exitmacro
        ; done
    .else

         position .set position + 1 ; increase index

        .if (position > .tcount({exp}))
            ; no more tok_typeens, not found, set pos to 0
            position .set - 1
            .exitmacro
        .endif

        ; no match yet:
        .ifnblank {openskip} ; we already increased the index, so check if we skipped an open bracket then expand a new macro to find close bracket honoring other brackets
            .if .xmatch({.mid(position-1,1,{exp})},{openskip})
        ;          .out .sprintf( "found ( at %d", position - 1)
                findidentstring {exp}, position , openskip, closeskip
        ;         .out .sprintf( "found ) at %d", position)
                position .set position + 1 ; skip the close bracket we just found
            .endif
        .endif

        .if .not (.blank( {altopenskip}))
            .if .xmatch({.mid(position-1,1,{exp})},{altopenskip})
                 ; .out .sprintf( "found [ at %d", position - 1)
                findidentstring {exp}, position
                 ; .out .sprintf( "found ] at %d", position)
                position .set position + 1 ; skip the close bracket we just found
            .endif
        .endif

        findidentstring {exp}, position, openskip, closeskip, altopenskip, altcloseskip

    .endif
.endmacro


; given position of left bracket, matches right bracket, skipping other bracket pairs of the same kind

.macro matchbrackets exp, position, openbracket, closebracket, openskip, closeskip

    ; assume position is at left bracket first time:
    position .set position + 1

    .if .xmatch({.mid(position,1,{exp})},{closebracket})
        .exitmacro
    .else
        ; no match yet:
        .ifnblank {openskip} ; if open bracket then expand a new macro to find close bracket honoring other open/closeskip
            .if .xmatch({.mid(position,1,{exp})},{openskip})
                matchbrackets {exp}, position, openskip, closeskip
            .endif
        .endif

        .if .xmatch({.mid(position,1,{exp})},{openbracket})
            matchbrackets {exp}, position, openbracket, closebracket, openskip, closeskip
        .endif

        .if (position >=  .tcount({exp}))
            ; no more tokens, not found, set pos to 0
            position .set 0 ; no matching closing bracket - not there or mismatched
            .exitmacro
        .endif

        matchbrackets {exp}, position, openbracket, closebracket, openskip, closeskip
    .endif
.endmacro

; stack macros:



.macro push stackname, value

    .ifndef ::.ident(.sprintf("_%s_STACKPOINTER", stackname))
        ::.ident(.sprintf("_%s_STACKPOINTER", stackname)) .set 0 ; define new stack
    .endif

    ::.ident( .sprintf("%s_%04X_",stackname,::.ident(.sprintf("_%s_STACKPOINTER", stackname)))) .set value

    ::.ident(.sprintf("_%s_STACKPOINTER", stackname))  .set ::.ident(.sprintf("_%s_STACKPOINTER", stackname))  + 1
.endmacro


.macro pop stackname, var ; puts a -1 in var if there is a problem

    .ifndef ::.ident(.sprintf("_%s_STACKPOINTER", stackname))   ; stack not defined
        var  .set  -1
    .elseif  ::.ident(.sprintf("_%s_STACKPOINTER", stackname)) - 1 < 0  ; or negative
        var .set  - 1
    .else
        ::.ident(.sprintf("_%s_STACKPOINTER", stackname)) .set ::.ident(.sprintf("_%s_STACKPOINTER", stackname)) - 1 ; dec stack pointer
        var  .set ::.ident( .sprintf("%s_%04X_",stackname,::.ident(.sprintf("_%s_STACKPOINTER", stackname))))    ; set value from stack
    .endif

.endmacro

.macro codeout I,O, indirect  ; wrap an ident into an instruction and do indirect syntax if needed

    .ifnblank O
        .ifnblank indirect
            I (O)
        .else
            I O
        .endif
    .else
        I
    .endif
.endmacro


; This is to incorporate any special syntax, like checking for operators that need to be preprocessed by a macro

.macro Process_macro_or_instruction I

    .local _op_, _op_pos_
    _op_ .set 0
    _op_pos_ .set 0

; strip outside enclosing brackets and do any special functions before calling macro or inst.

    ; first strip any enclosing brackets remaining:
    checkbrackets {I} ; Uses _AND_POS_ and _OR_POS_ as temps

    .if ___HL::_AND_POS_ = -1 ; indicates we matched outside bracket, remove
        Process_macro_or_instruction {.mid(1,___HL::_OR_POS_ -1,{I})}
        .exitmacro
    .endif

    ; integrated support for comparison macros:
    ; first check for operators that indicate unsigned comparison : > < <> >= <=
    find_compare_operator {I}, _op_pos_, _op_

    .if _op_pos_
        ; .macro comp exp, withreg, _found_op_, _found_op_pos_
        comp  {I},,_op_,_op_pos_
    .else
        ; check for assigment op :=
        findposition {I}, :=, _op_pos_
        .if _op_pos_
            mb {I}
            ___HL::_FOUND_IDENTIFIER_ .set 1
        .else
            ; check for math operator:
            ; .macro find_mathlogic_operator exp, _position_, _op_
            find_mathlogic_operator {I}, _op_pos_, _op_

            .if _op_

                mb {I},,1,_op_pos_,_op_
                ___HL::_FOUND_IDENTIFIER_ .set 1
            .else

                ; try to quickly determine if it is a implied lda, an explicit instruction or must be a macro:

                findidentstring {I}, _op_pos_

                .if _op_pos_ = - 1 ; no string-like thing found:
                    .if .xmatch(#,{.mid(0,1,{I})}) ; could be immediate:
                        do_array_syntax lda, {I}
                        ___HL::_FOUND_IDENTIFIER_ .set 1
                        _MB_MACRO_::Last_reg_used .set _MB_MACRO_::REGA ; tell other routines we matched reg a
                        .exitmacro
                    .else
                        .error "Unknown expresssion."
                        .exitmacro
                    .endif
                .endif

                ; here, some kind of string found:

                .if (.defined( ::.ident(.string(.mid(_op_pos_,1,{I})))) .or .defined( .ident(.string(.mid(_op_pos_,1,{I}))) ))

                        ; if so it is an identifier not a macro or instruction
                        do_array_syntax lda, {I}
                        ___HL::_FOUND_IDENTIFIER_ .set 1
                        _MB_MACRO_::Last_reg_used .set _MB_MACRO_::REGA ; tell other routines we matched reg a

                .elseif .strlen(.string(.mid(0,1,{I}))) = 3 ; this is as close as we can get to quickly determining if this is an instruction for now until ca65 has a function for it

                    do_array_syntax .mid(0,1,{I}), {.mid(1,.tcount({I}) - 1,{I})} ; do instruction
                    ___HL::_FOUND_IDENTIFIER_ .set 1

                .else
                    I  ; call as a macro, if ca65 has a .is_macro_or_instruction() function, errors could be nicer
                       ; the macro must use set_flag_test if the expression does not use ==, or !=
                .endif

            .endif
        .endif
    .endif

.endmacro

.macro codeout_recursive I

; I can contain a macro and options or any number of instructions seperated by :
; it must end with a macro that sets the flags via set_flag_test or set the flags with ==, or !=

.local _negate_flags, equal_pos, _check_plus_pos, _colon_pos_

    _colon_pos_     .set 0
    equal_pos     .set 0

    _negate_flags .set 0

    findposition {I}, : ,_colon_pos_ , (,)

    .if _colon_pos_
        codeout_recursive {.mid(0, _colon_pos_, {I}) }
        codeout_recursive {.mid(_colon_pos_+1, .tcount({I}) - _colon_pos_ - 1,{I})}

    .else

        ; find == for defining flags with inline code:
        findposition {I},=,equal_pos , (,)
        ; only match double equal:
        .if equal_pos <> 0
            .if  .not .xmatch( {.mid(equal_pos+ (equal_pos = - 1) +1,1,{I})},=) ; If second equal doesn't exsit
                equal_pos .set 0
            .endif
        .endif

        .if .not equal_pos
            ; equal_pos .set 0
            ; find != for defining flags with inline code:
            findposition {I},!,equal_pos, (,)
            ; only match !=
            .if .not .xmatch( {.mid(equal_pos+ (equal_pos = - 1) +1,1,{I})},=) ; If second equal doesn't exsit
                equal_pos .set 0
            .else ; match found:
                _negate_flags .set 1
            .endif
        .endif

        .if equal_pos
            codeout_recursive {.mid(0, equal_pos, {I}) }

            .if _negate_flags
                .define _CPU_FLAGS_ _HL_MACRO_USER_NOT_DEFINITION1_ .mid(equal_pos + 2 ,.tcount({I}) - equal_pos -2,{I})
            .else
                .define _CPU_FLAGS_ .mid(equal_pos + 2 ,.tcount({I}) - equal_pos -2,{I})
            .endif

            ___HL::__MACRO_FUNCTION_COMPLETE_ .set 1 ; signal that flags were set properly

        .else  ; no plus, no equal,  should be a macro:
            Process_macro_or_instruction {I}
        .endif
    .endif

.endmacro


.macro Process_Expression express, flag, flagisset, negatefunction

; Find a value for which flag we are checking and return:
; format of express = [no OR not] flag 'set' OR 'clear'  ['set' OR 'clear' ]
; If flag is not recognized, attempt to expand the macro and test if it completed
; if so, test the flag it set

.local _n_

    _n_ .set  (  (.xmatch(.left(1, {express}) , _HL_MACRO_USER_NOT_DEFINITION1_)) .or (.xmatch(.left(1, {express}) , _HL_MACRO_USER_NOT_DEFINITION2_))  ) ; not or no prefix

    ; handle multiple 'not's
    .if _n_
        .ifnblank negatefunction ; double negative, recursive call, no negate
            Process_Expression {.mid(_n_,.tcount({express}) - _n_ , {express})}, flag, flagisset
        .else
            Process_Expression {.mid(_n_,.tcount({express}) - _n_ , {express})}, flag, flagisset, 1
        .endif
        .exitmacro
    .endif

    ; Which flag are we testing? HAS to be a real flag, with the exception of G = greater

    flag .set (.xmatch(.mid(0,1, {express}) , C)) * 1 + (.xmatch(.mid(0,1, {express}) , Z)) * 2 + (.xmatch(.mid(0,1,{express}) , N)) * 3 + (.xmatch(.mid(0,1,{express}) , V)) * 4 + (.xmatch(.mid(0,1,{express}) , G)) * 5


    ; if No flag found, Assume we should be calling a macro, or executing code or testing for NOT ZERO
    ; this is all determined by codeout_recursive which will signal back if it set the flag to test or not
    .if (flag = 0)

        ___HL::__MACRO_FUNCTION_COMPLETE_ .set 0
        ___HL::_FOUND_IDENTIFIER_ .set 0

        codeout_recursive {express}

        .if .not ___HL::__MACRO_FUNCTION_COMPLETE_
            .if ___HL::_FOUND_IDENTIFIER_ ; if found something that should be okay to default to non-zero is true behaviour
                .define _CPU_FLAGS_ Z clear
            .else
                .error .sprintf("%s %s","Error in or unrecognized conditional function or instruction:", .string(.mid(0,1, {express})))
                .exitmacro
            .endif
        .endif

        Process_Expression {_CPU_FLAGS_}, flag, flagisset, negatefunction
        .undefine _CPU_FLAGS_

        .exitmacro

    .endif

    __flag_set_default .set (.xmatch(.mid(0+1,1, {express}), set)) * 1 + (.xmatch(.mid(0+1,1, {express}), clear)) *  -1

    .ifnblank .mid(0+2,1, {express}) ; user added clear or set, if token exsits here it must be a clear or set, any other is error
        __invert_flag             .set .xmatch(.mid(0+2,1, {express}), clear) *  -1  + .xmatch(.mid(0+2,1, {express}), set) * 1
    .else
        __invert_flag             .set 1 ; no clear or set added, that's okay make it a 1
    .endif
    ; __invert_flag            .set  __invert_flag * (0 * -2 + 1)
    .ifnblank negatefunction ; support for recursive expansion and honor the original NOT operator
        __invert_flag .set __invert_flag   * (negatefunction * -2 + 1)
    .endif

    flagisset .set ((__flag_set_default * __invert_flag) > 0)     ; This will always result in a negative or positive, positive means if cpu flag is set then we will branch

    .if (flag = 0)
        .error .concat( "Unknow flag: ", .string(.mid(0,1, express)))
    .endif

    .if (__invert_flag * __flag_set_default) = 0
        .error .concat ( "Unknow flag condition: " , .string(.mid(0+1 + (__flag_set_default <> 0),1, {express})))
    .endif


.endmacro

.macro break
    ; _CURRENT___LOOP_TYPE_ =
    ; _NO_LOOP_
    ; _DO_WHILE_
    ; _WHILE_DO_

    pop "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_
    push "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_

    .if (::_CURRENT___LOOP_TYPE < 0 )
        ___HL::_CURRENT___LOOP_TYPE_ .set  ___CURRENT_LOOP_::_NO_LOOP_
        .error "No loop to break out of."
    .elseif ___HL::_CURRENT___LOOP_TYPE_ = ___CURRENT_LOOP_::_DO_WHILE_
        ; peek at value
        pop "_DO_WHILE_STACK_", ___HL::_DO_WHILE__WHILE_label
        push "_DO_WHILE_STACK_", ___HL::_DO_WHILE__WHILE_label
        jmp .ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_", ___HL::_DO_WHILE__WHILE_label))
    .elseif ___HL::_CURRENT___LOOP_TYPE_ = ___CURRENT_LOOP_::_WHILE_DO_
        pop "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__END_label
        push "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__END_label, ::_WHILE_ENDWHILE
        jmp .ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__WHILE_label))
    .endif


.endmacro


.macro checkORAND express; 'or' was found, make sure no 'or' after any 'and'

    findposition {express}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
    .if (___HL::_AND_POS_ < ___HL::_OR_POS_)
        .error " AND before OR in conditional expression, will not be parsed properly."
        .exitmacro
    .elseif .not ___HL::_OR_POS_
        checkORAND {express}
    .endif

.endmacro


; *************************IF**********************

; User is expected to only pass express, and nothing more
.macro if express

    findposition {express}, _HL_MACRO_USER_GOTO_DEFINITION_,___HL::_OR_POS_ , (,) ; or _OR_POS_ as temp

    .if ___HL::_OR_POS_
        if_macro {.mid(0,___HL::_OR_POS_ ,{express})},,,,,{.mid(___HL::_OR_POS_+1, .tcount({express}) - ___HL::_OR_POS_-1,{express})}
    .else ; no goto:

        findposition {express}, break,___HL::_OR_POS_ , (,) ; or _OR_POS_ as temp
        .if ___HL::_OR_POS_ ; if <exp> break

            pop "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_
            push "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_

            .if (___HL::_CURRENT___LOOP_TYPE_ < 0 )
                ___HL::_CURRENT___LOOP_TYPE_ .set  ___CURRENT_LOOP_::_NO_LOOP_
                .error "No loop to break out of in IF statement."
            .elseif ___HL::_CURRENT___LOOP_TYPE_ = ___CURRENT_LOOP_::_DO_WHILE_
                ; peek at value
                pop "_DO_WHILE_STACK_", ___HL::_DO_WHILE__WHILE_label
                push "_DO_WHILE_STACK_", ___HL::_DO_WHILE__WHILE_label
                if_macro {.mid(0,___HL::_OR_POS_ ,{express})},,,,,.ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_", ___HL::_DO_WHILE__WHILE_label))

            .elseif ___HL::_CURRENT___LOOP_TYPE_ = ___CURRENT_LOOP_::_WHILE_DO_
                ; peek at value
                pop "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__END_label
                push "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__END_label
                if_macro {.mid(0,___HL::_OR_POS_ ,{express})},,,,,.ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__WHILE_label))
            .endif

        .else ; no break, no goto:
            if_macro {express}
        .endif
    .endif

.endmacro

.macro if_macro express, lastrecursive, recursive, leftsideOR, leftsideAND, gotolabel

    ; First, if there are brackets on the very outside of the expression, remove them and try again
    checkbrackets {express} ; Uses _AND_POS_ and _OR_POS_ as temps
    ; try again with brackets removed
    .if ___HL::_AND_POS_ = -1 ; indicates we matched outside bracket
            
            if_macro {.mid(1,___HL::_OR_POS_ -1,{express})}, lastrecursive, recursive, leftsideOR, leftsideAND, gotolabel
            ___HL::all_or_before_and_flag .set 0
        .exitmacro
    .endif
    ; end brackets

    .if (.blank (recursive) .and .blank(lastrecursive)) ; this means only first call (in a set of recursive calls) to this macro does this block get executed
        ___HL::_IF_ENDIF__IF_label .set ___HL::_IF_ENDIF__IF_label + 1
        ___HL::all_or_before_and_flag .set 0
        .ifblank gotolabel ; do not save to endif stack if there is no endif
            push "IF_ENDIF_STACK",___HL::_IF_ENDIF__IF_label
        .endif
    .endif

    ___HL::_AND_POS_ .set 0
    ___HL::_OR_POS_ .set 0

    ; findposition exp, tok, position, openskip, closeskip

    findposition {express}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
    .if ___HL::_OR_POS_

        ___HL::_OR_POS_copy  .set  ___HL::_OR_POS_

        .if .not ___HL::all_or_before_and_flag
            findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and in the entire enrty
            .if ___HL::_AND_POS_
                checkORAND {express} ; make sure there are no ORs after the first AND
                ___HL::all_or_before_and_flag .set 1
            .endif
        .endif

        if_macro {.mid(0,___HL::_OR_POS_copy,{express})},,1,1,, gotolabel

        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                if_macro {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},1, , , , gotolabel  ; last recursive call
        .else
                if_macro {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},,1, , , gotolabel
        .endif

        .exitmacro
    .endif


    findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and
    .if ___HL::_AND_POS_

        ___HL::_AND_POS_copy  .set ___HL::_AND_POS_

        if_macro {.mid(0,___HL::_AND_POS_,{express})},,1,,1, gotolabel

        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                if_macro  {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},1, , , , gotolabel
        .else
                if_macro {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},,1, , , gotolabel
        .endif
        .exitmacro
    .endif

    Process_Expression {express}, ___HL::_IF_ENDIF__flagnum, ___HL::_IF_ENDIF__flagset

    .ifblank gotolabel
        ; normal if-endif
        .if (.not .blank(leftsideOR)) .or ( ___HL::_LONG_JUMP_ .and .blank(recursive))
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), .ident ( .sprintf ("%s%04X", "IF_CODE_BLOCK_START_LABEL_", ___HL::_IF_ENDIF__IF_label))
        .else
            ___HL::_IF_ENDIF__flagset .set .not  ___HL::_IF_ENDIF__flagset
            .if (.not .blank(leftsideAND)) .and ___HL::_LONG_JUMP_ ; If Leftside and and long jump on
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), .ident ( .sprintf ("%s%04X", "_IF_ENDIF_JMP_", ___HL::_IF_ENDIF__IF_label))
            .else
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label))
            .endif
        .endif

    .else
        ; branch to label:
        .if (.not .blank(leftsideAND)) .or ( ___HL::_LONG_JUMP_ .and .blank(recursive))
            ___HL::_IF_ENDIF__flagset .set .not  ___HL::_IF_ENDIF__flagset
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), .ident ( .sprintf ("%s%04X", "IF_CODE_BLOCK_START_LABEL_", ___HL::_IF_ENDIF__IF_label))
        .else
            .if (.not .blank(leftsideOR)) .and ___HL::_LONG_JUMP_
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), .ident ( .sprintf ("%s%04X", "_IF_ENDIF_JMP_", ___HL::_IF_ENDIF__IF_label))
            .else
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_IF_ENDIF__flagset, ___HL::_IF_ENDIF__flagnum )), gotolabel
            .endif
        .endif

    .endif

    .if .blank (recursive) ; no more branches:
        .if  ___HL::_LONG_JUMP_

            .ident ( .sprintf ("%s%04X", "_IF_ENDIF_JMP_", ___HL::_IF_ENDIF__IF_label)):

            .if ___HL::_BRANCH_LENGTH_CHECKING_
                .assert (.ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label)) ) - (.ident ( .sprintf ("%s%04X", "IF_CODE_BLOCK_START_LABEL_", ___HL::_IF_ENDIF__IF_label))) > 127, warning, .sprintf("JMP in IF macro could have been reached with a branch at IF block %d",___HL::_IF_ENDIF__IF_label)
            .endif

            .ifblank gotolabel
                jmp .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label))
            .else
                jmp gotolabel
            .endif

        .endif

        .ident ( .sprintf ("%s%04X", "IF_CODE_BLOCK_START_LABEL_", ___HL::_IF_ENDIF__IF_label)):
    .endif

.endmacro

; Create a label that will point to the next ENDIF and create an ENDIF/IF for the ELSE

.macro else flag

.local __flagnum, __flagset


        ___HL::_IF_ENDIF__IF_label .set ___HL::_IF_ENDIF__IF_label + 1 ; add one to IF count

        .ifblank flag
            ; just do unconditional jump
            jmp .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label)) ; jump to the next endif (this is like a IF false - it will always jump)
        .else
            ; process flag and jump to next 'endif' using user defined unconditional branch
            Process_Expression {flag}, __flagnum, __flagset
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", __flagset, __flagnum )), .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label))
        .endif

        endif    ; end if for the IF before this

        push "IF_ENDIF_STACK",___HL::_IF_ENDIF__IF_label ; next endif will use this label



.endmacro

.macro elseif  express

    ___HL::_IF_ENDIF__IF_label .set ___HL::_IF_ENDIF__IF_label + 1 ; add one to IF count

    jmp .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__IF_label)) ; jump to the next endif (this is like a IF false - it will always jump)
    endif    ; end if for the IF before this

    ___HL::_IF_ENDIF__IF_label .set ___HL::_IF_ENDIF__IF_label - 1

    if {express}

.endmacro


; Pop if address from the IF stack and make a label
.macro endif

    pop "IF_ENDIF_STACK",___HL::_IF_ENDIF__END_label

    .if ___HL::_IF_ENDIF__END_label  < 0
        .error "ENDIF or ELSE without IF"
        .fatal ""
    .endif

    .ident ( .sprintf ("%s%04X", "_END_IF_", ___HL::_IF_ENDIF__END_label)):

.endmacro



; *************************END IF**********************

.macro do ;  for do - while

    ___HL::_DO_WHILE__DO_label .set ___HL::_DO_WHILE__DO_label + 1

    push "_DO_WHILE_STACK_",___HL::_DO_WHILE__DO_label
    .ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__DO_label)):

    ; break support:

    push "_BREAK_FROM_LOOP_STACK_" , ___CURRENT_LOOP_::_DO_WHILE_

.endmacro

.macro while_DO_WHILE  express, lastrecursive, recursive, leftsideOR, leftsideAND, INV

.local long_jump


    ___HL::_AND_POS_ .set 0
    ___HL::_OR_POS_ .set 0
    checkbrackets {express} ; Uses _AND_POS_ and _OR_POS_ as temps
    ; try again with brackets removed
    .if ___HL::_AND_POS_ = -1
         while_DO_WHILE {.mid(1,___HL::_OR_POS_ -1,{express})}, lastrecursive, recursive, leftsideOR, leftsideAND, INV
        .exitmacro
    .endif
    ; end brackets

    .if  (.blank (recursive) .and .blank(lastrecursive)) ; this will only be executed on first call to this macro
        pop "_DO_WHILE_STACK_", ___HL::_DO_WHILE__WHILE_label
        .if (___HL::_DO_WHILE__WHILE_label < 0)  ; if accessing the stack will resulted in a negative value, there are too many whiles
            .error "WHILE/UNTIL without DO/REPEAT."
            .fatal ""
        .endif

        pop "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_

    .endif

        ; do and / or

    ___HL::_AND_POS_ .set 0
    ___HL::_OR_POS_ .set 0

    ; findposition exp, tok, position, openskip, closeskip

    findposition {express}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
    .if ___HL::_OR_POS_

        ___HL::_OR_POS_copy  .set  ___HL::_OR_POS_

        .if .not ___HL::all_or_before_and_flag
            findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for AND in the entire entry
            .if ___HL::_AND_POS_
                checkORAND {express} ; make sure there are no ORs after the first AND
                ___HL::all_or_before_and_flag .set 0
            .endif
        .endif

        while_DO_WHILE  {.mid(0,___HL::_OR_POS_copy,{express})},,1,1,,INV

        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                while_DO_WHILE   {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},1 ,,,,INV ; last recursive call
        .else
                while_DO_WHILE  {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},,1,,,INV
        .endif

        .exitmacro
    .endif

    findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and
    .if ___HL::_AND_POS_

        ___HL::_AND_POS_copy  .set ___HL::_AND_POS_

        while_DO_WHILE {.mid(0,___HL::_AND_POS_,{express})},,1,,1,INV

        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                while_DO_WHILE {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},1, , , ,INV
        .else
                while_DO_WHILE {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},,1, , ,INV
        .endif
        .exitmacro
    .endif

    ; We only get here if there are no AND or ORS left in the current expansion:

    ; guess if we need long jump:
    .if .not ___HL::_LONG_JUMP_
        .if      *  - .ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__WHILE_label)) > 128 ; for sure we need a long jump, but we might need one even if this fails if too many branches
            long_jump .set 1
        .else
            long_jump .set 0
        .endif
    .else
        long_jump .set ___HL::_LONG_JUMP_
    .endif

    Process_Expression {express}, ___HL::_DO_WHILE__flagnum, ___HL::_DO_WHILE__flagset

;-------------------------------------------------PROCESS UNTIL
    .ifnblank INV ; this block of code copied from IF since an UNTIL is just like an IF with different labels
            ; IF True BRANCH out of loop

        .if (.not .blank(leftsideOR)) .or ( long_jump .and .blank(recursive))
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_", ___HL::_DO_WHILE__WHILE_label))
        .else
            ___HL::_DO_WHILE__flagset .set .not  ___HL::_DO_WHILE__flagset
            .if (.not .blank(leftsideAND)) .and long_jump ; If Leftside and and long jump on
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_WHILE_JMP_", ___HL::_DO_WHILE__WHILE_label))
            .else
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__WHILE_label))
            .endif
        .endif

;-------------------------------------------------PROCESS WHILE: ( IF true GOTO begining)
    .else

        .if (.not .blank(leftsideAND)) .or ( long_jump .and .blank(recursive))
            ___HL::_DO_WHILE__flagset .set .not  ___HL::_DO_WHILE__flagset
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_", ___HL::_DO_WHILE__WHILE_label))
        .else
            .if (.not .blank(leftsideOR)) .and long_jump
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_WHILE_JMP_", ___HL::_DO_WHILE__WHILE_label))
            .else
                codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_DO_WHILE__flagset, ___HL::_DO_WHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__WHILE_label))
            .endif
        .endif

    .endif

    .ifblank recursive
        .if long_jump
            .ident ( .sprintf ("%s%04X", "_DO_WHILE_JMP_", ___HL::_DO_WHILE__WHILE_label)) :

            .if ___HL::_BRANCH_LENGTH_CHECKING_
                .assert (.ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__WHILE_label)) ) - (.ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_", ___HL::_DO_WHILE__WHILE_label))) > 127, warning, .sprintf("JMP in do-while loop could have been reached with a branch at do-while block %d",___HL::_DO_WHILE__WHILE_label)
            .endif

            jmp .ident ( .sprintf ("%s%04X", "_DO_WHILE_", ___HL::_DO_WHILE__WHILE_label))
        .endif
        .ident ( .sprintf ("%s%04X", "_DO_EXIT___LOOP_CONTINUE_",___HL::_DO_WHILE__WHILE_label)):
    .endif

.endmacro

.macro while_WHILE_ENDWHILE express, lastrecursive, recursive, leftsideOR, leftsideAND, INV
; while for starting a while - endwhile block, if N exists it will negate the test to emulate an 'until'

    ; First, if there are brackets on the very outside of the expression, remove them and try again

    checkbrackets {express} ; Uses _AND_POS_ and _OR_POS_ as temps
    ; try again with brackets removed
    .if ___HL::_AND_POS_ = -1 ; indicates we matched outside bracket
        while_WHILE_ENDWHILE {.mid(1,___HL::_OR_POS_ -1,{express})}, lastrecursive, recursive, leftsideOR, leftsideAND
        .exitmacro
    .endif
    ; end brackets

    .if (.blank (recursive) .and .blank(lastrecursive))
        ___HL::_WHILE_ENDWHILE__WHILE_label .set ___HL::_WHILE_ENDWHILE__WHILE_label + 1
        push "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__WHILE_label
        .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_REPEAT_", ___HL::_WHILE_ENDWHILE__WHILE_label)):
        ; break support:
        push "_BREAK_FROM_LOOP_STACK_" , ___CURRENT_LOOP_::_WHILE_DO_

    .endif

    ___HL::_AND_POS_ .set 0
    ___HL::_OR_POS_ .set 0

    ; findposition exp, tok, position, openskip, closeskip

    findposition {express}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
    .if ___HL::_OR_POS_

        ___HL::_OR_POS_copy  .set  ___HL::_OR_POS_

        .if .not ___HL::all_or_before_and_flag
            findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and in the entire enrty
            .if ___HL::_AND_POS_
                checkORAND {express} ; make sure there are no ORs after the first AND
                ___HL::all_or_before_and_flag .set 0
            .endif
        .endif

        while_WHILE_ENDWHILE {.mid(0,___HL::_OR_POS_copy,{express})},,1,1

        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                while_WHILE_ENDWHILE  {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},1  ; last recursive call
        .else
                while_WHILE_ENDWHILE {.mid(___HL::_OR_POS_copy+1, .tcount({express})-___HL::_OR_POS_copy-1,{express})},,1
        .endif

        .exitmacro
    .endif


    findposition {express}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and
    .if ___HL::_AND_POS_

        ___HL::_AND_POS_copy  .set ___HL::_AND_POS_

        while_WHILE_ENDWHILE {.mid(0,___HL::_AND_POS_,{express})},,1,,1

        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_OR_DEFINITION_, ___HL::_OR_POS_, (,) ; look for or
        findposition {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})}, _HL_MACRO_USER_AND_DEFINITION_, ___HL::_AND_POS_, (,) ; look for and

        .if  (.not  ___HL::_OR_POS_ ) .and  (.not ___HL::_AND_POS_)
                while_WHILE_ENDWHILE {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},1
        .else
                while_WHILE_ENDWHILE {.mid(___HL::_AND_POS_copy+1, .tcount({express})-___HL::_AND_POS_copy-1,{express})},,1
        .endif
        .exitmacro
    .endif

    Process_Expression {express}, ___HL::_WHILE_ENDWHILE__flagnum, ___HL::_WHILE_ENDWHILE__flagset

    .if (.not .blank(leftsideOR)) .or ( ___HL::_LONG_JUMP_ .and .blank(recursive))
        codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_WHILE_ENDWHILE__flagset, ___HL::_WHILE_ENDWHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_CODE_BLOCK_START_", ___HL::_WHILE_ENDWHILE__WHILE_label))
    .else
        ___HL::_WHILE_ENDWHILE__flagset .set .not  ___HL::_WHILE_ENDWHILE__flagset
        .if (.not .blank(leftsideAND)) .and ___HL::_LONG_JUMP_ ; If Leftside and and long jump on
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_WHILE_ENDWHILE__flagset, ___HL::_WHILE_ENDWHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_JMP_", ___HL::_WHILE_ENDWHILE__WHILE_label))
        .else
            codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", ___HL::_WHILE_ENDWHILE__flagset, ___HL::_WHILE_ENDWHILE__flagnum )), .ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__WHILE_label))
        .endif
    .endif


    .if .blank (recursive) ; no more branches:
        .if  ___HL::_LONG_JUMP_
            .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_JMP_", ___HL::_WHILE_ENDWHILE__WHILE_label)):

            .if ___HL::_BRANCH_LENGTH_CHECKING_
                .assert (.ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__WHILE_label)) ) - (.ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_CODE_BLOCK_START_", ___HL::_WHILE_ENDWHILE__WHILE_label))) > 127, warning, .sprintf("JMP in IF macro could have been reached with a branch at IF block %d",___HL::_WHILE_ENDWHILE__WHILE_label)
            .endif

            jmp .ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__WHILE_label))
        .endif

        .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_CODE_BLOCK_START_", ___HL::_WHILE_ENDWHILE__WHILE_label)):

    .endif


.endmacro

.macro endwhile flag

.local __flagnum, __flagset

    pop "WHILE_ENDWHILE_STACK_",___HL::_WHILE_ENDWHILE__END_label

    .if ___HL::_WHILE_ENDWHILE__END_label < 0 ; if accessing the stack will result in a negative value
        .error "ENDWHILE without WHILE"
        .fatal ""
    .endif

    .ifblank flag
        jmp .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_REPEAT_", ___HL::_WHILE_ENDWHILE__END_label))
    .else
        Process_Expression {flag}, __flagnum, __flagset
        codeout  .ident (.sprintf("___CPU_FLAG_COMMAND_%01X_%01X", __flagset, __flagnum )), .ident ( .sprintf ("%s%04X", "_WHILE_ENDWHILE_REPEAT_", ___HL::_WHILE_ENDWHILE__END_label))
    .endif

    .ident ( .sprintf ("%s%04X", "_WHILE_EXIT___LOOP_CONTINUE_", ___HL::_WHILE_ENDWHILE__END_label)):

    ; break support
    pop "_BREAK_FROM_LOOP_STACK_" , ___HL::_CURRENT___LOOP_TYPE_


.endmacro

.macro while express, INV

    .if .xmatch(.right(1,{express}),do) ; Is a "while <cond> do" -endwhile
        while_WHILE_ENDWHILE {.mid (0,.tcount({express})-1,{express})} ,,,,,INV ; send without 'do' on the end
    .else
        while_DO_WHILE {express},,,,,INV
    .endif

.endmacro


.macro until express
    .if .xmatch(.right(1,{express}),do) ; Is a "while <cond> do" -endwhile
        .error "until (condition) do - enduntil not supported in this version"
        .fatal
    .endif
    while {express},1 ; invert while test for repeat-until
.endmacro

.macro enduntil I
    endwhile I
.endmacro

.macro repeat
    do
.endmacro


.macro set_flag_test testflags
    ___HL::__MACRO_FUNCTION_COMPLETE_ .set 1
    .define _CPU_FLAGS_ testflags
.endmacro



;*************************************************************************************************************************


; function for use with the above:

.scope _COMP_MACRO_
    INVALID = 0
    GREATER = 1
    LESS = -1
    GREATEREQUAL = 2
    LESSEQUAL = -2
    EQUAL = 3
    NOTEQUAL = 4

    REGX = 1
    REGY = 2
    REGA = 3
.endscope

.macro find_compare_operator exp, _pos_, _op_

    findposition {exp}, =, _pos_,(,)
    .if _pos_
        _op_ .set _COMP_MACRO_::EQUAL
    .else
        findposition {exp}, <>, _pos_,(,)
        .if _pos_
            _op_ .set _COMP_MACRO_::NOTEQUAL
        .else
            findposition {exp}, >=, _pos_,(,)
            .if _pos_
                _op_ .set _COMP_MACRO_::GREATEREQUAL
            .else
                findposition {exp}, <=, _pos_,(,)
                .if _pos_
                    _op_ .set _COMP_MACRO_::LESSEQUAL
                .else
                    findposition {exp}, >, _pos_,(,)
                    .if _pos_
                        _op_ .set _COMP_MACRO_::GREATER
                    .else
                        findposition {exp}, <, _pos_,(,)
                        .if _pos_
                            _op_ .set _COMP_MACRO_::LESS
                        .endif
                    .endif
                .endif
            .endif
        .endif
    .endif

.endmacro


.macro comp exp, withreg, _found_op_, _found_op_pos_

; withreg not implemented yet
; if

.local _pos_, _pos2_, _op_

    checkbrackets {exp} ; Uses _AND_POS_ and _OR_POS_ as temps
    ; try again with brackets removed
    .if ___HL::_AND_POS_ = -1
        comp {.mid(1,___HL::_OR_POS_ -1,{exp})}
        .exitmacro
    .endif

    .ifblank _found_op_
        _pos_ .set _COMP_MACRO_::INVALID
        _op_ .set _COMP_MACRO_::INVALID

        find_compare_operator {exp}, _pos_, _op_

        .if (.not _pos_ ).or (.not _op_)
            .error "Unknow operator in comparison macro."
            .fatal ""
        .endif
    .else
        ; info was passed to save processing time:
        _pos_ .set _found_op_pos_
        _op_ .set _found_op_

    .endif

    ; Check for register on the right side of the expression and adjust (pretend it is always on the left)

    _left_ .set _COMP_MACRO_::INVALID
    .if .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),x) ; reg x
        _left_ .set _COMP_MACRO_::REGX
    .elseif .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),y) ; reg y
        _left_ .set _COMP_MACRO_::REGY
    .elseif .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),a) ; reg a
        _left_ .set _COMP_MACRO_::REGA
    .endif

    .if  .not (_left_ = _COMP_MACRO_::INVALID)        ; if we found a match on the right side, pretend it is on the left by switching _op_
        .if _op_ < 3
            _op_ .set _op_ *  -1 ; don't switch equal or not equal
        .endif
        _right_pos_    .set 0
        _right_pos_count_ .set _pos_
    .else    ; right side normal values:
        _right_pos_    .set _pos_ + 1
        _right_pos_count_ .set .tcount({exp}) - _pos_  - 1
    .endif


    .if _left_ = _COMP_MACRO_::INVALID ;  There could be a register on the left side of the expression- find it
        .if .xmatch(.mid(0,_pos_,{exp}),x) ; reg x
            _left_ .set _COMP_MACRO_::REGX
        .elseif .xmatch(.mid(0,_pos_,{exp}),y) ; reg y
            _left_ .set _COMP_MACRO_::REGY
        .elseif .xmatch(.mid(0,_pos_,{exp}),a) ; reg a
            _left_ .set _COMP_MACRO_::REGA
        .endif
    .endif

    .if _left_ = _COMP_MACRO_::INVALID ; no registers found, so first we will load a register, or eval an expression
        ; .ifnblank withreg
            ; .if xmatch(withreg, x)
                ; ldx .mid(0,_pos_,{exp})
                ; _left_ .set _COMP_MACRO_::REGX
            ; .elseif xmatch(withreg, y)
                ; ldy .mid(0,_pos_,{exp})
                ; _left_ .set _COMP_MACRO_::REGY
            ; .else
                ; do_array_syntax lda, {.mid(0,_pos_,{exp})}
                ; _left_ .set _COMP_MACRO_::REGA
            ; .endif

        _MB_MACRO_::Last_reg_used .set 0

        Process_macro_or_instruction {.mid(0,_pos_,{exp})}

        .if _MB_MACRO_::Last_reg_used
            _left_ .set _MB_MACRO_::Last_reg_used
        .else

            ; instructions that comp will autmatically recognize, add more here if needed:

            .if .xmatch(.mid(0,1,{exp}),lda)
                _left_ .set _COMP_MACRO_::REGA
            .elseif .xmatch(.mid(0,1,{exp}),ldx)
                _left_ .set _COMP_MACRO_::REGX
            .elseif .xmatch(.mid(0,1,{exp}),ldy)
                _left_ .set _COMP_MACRO_::REGY
                
            .elseif .xmatch(.mid(0,1,{exp}),inx)
                _left_ .set _COMP_MACRO_::REGX
            .elseif .xmatch(.mid(0,1,{exp}),dex)
                _left_ .set _COMP_MACRO_::REGX
            .elseif .xmatch(.mid(0,1,{exp}),iny)
                _left_ .set _COMP_MACRO_::REGY
            .elseif .xmatch(.mid(0,1,{exp}),dey)
                _left_ .set _COMP_MACRO_::REGY
            .endif
        .endif

        .if .not _left_
            .error "Unknown register to use in comparision macro."
        .endif

    .endif

    .if _left_ = _COMP_MACRO_::REGX
        do_array_syntax cpx, {.mid(_right_pos_,_right_pos_count_, {exp})}
    .elseif _left_ = _COMP_MACRO_::REGY
        do_array_syntax cpy, {.mid(_right_pos_,_right_pos_count_, {exp})}
    .elseif _left_ = _COMP_MACRO_::REGA
        do_array_syntax cmp, {.mid(_right_pos_,_right_pos_count_, {exp})}
    .endif

    .if _op_ = _COMP_MACRO_::GREATER
        set_flag_test G set
    .elseif _op_ = _COMP_MACRO_::LESS
        set_flag_test C clear
    .elseif _op_ = _COMP_MACRO_::EQUAL
        set_flag_test Z set
    .elseif _op_= _COMP_MACRO_::GREATEREQUAL
        set_flag_test C set
    .elseif _op_= _COMP_MACRO_::LESSEQUAL
        set_flag_test G clear
    .elseif _op_= _COMP_MACRO_::NOTEQUAL
        set_flag_test Z clear
    .endif

.endmacro

.macro scomp exp

; code algorithm for signed comparision from :
; Beyond 8-bit Unsigned Comparisons by Bruce Clark , http://www.6502.org/tutorials/compare_beyond.html
;
; Not thoroughly tested, should work fine

.local jmp_over, jmp_over2

    _pos_ .set _COMP_MACRO_::INVALID
    _op_ .set _COMP_MACRO_::INVALID

    find_compare_operator {exp}, _pos_, _op_

    .if (.not _pos_ ).or (_op_ = _COMP_MACRO_::INVALID)
        .error "Unknow operator in signed comparison macro."
        .fatal ""
    .endif

    ; Check for register on the right side of the expression and adjust (pretend it is always on the left)

    _left_ .set _COMP_MACRO_::INVALID
    .if .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),x) ; reg x
        _left_ .set _COMP_MACRO_::REGX
    .elseif .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),y) ; reg y
        _left_ .set _COMP_MACRO_::REGY
    .elseif .xmatch(.mid((_pos_+1), .tcount({exp}) - _pos_  - 1 , {exp}),a) ; reg a
        _left_ .set _COMP_MACRO_::REGA
    .endif

    .if  .not (_left_ = _COMP_MACRO_::INVALID)        ; if we found a match on the right side, pretend it is on the left by switching _op_
        .if _op_ < 3
            _op_ .set _op_ *  -1 ; only switch if not ( equal or not equal)
        .endif
        _right_pos_    .set 0
        _right_pos_count_ .set _pos_
    .else    ; right side normal values:
        _right_pos_    .set _pos_ + 1
        _right_pos_count_ .set .tcount({exp}) - _pos_  - 1
    .endif

    .if _left_ = _COMP_MACRO_::INVALID ;  There could be a register on the left side of the expression- find it
        .if .xmatch(.mid(0,_pos_,{exp}),x) ; reg x
            _left_ .set _COMP_MACRO_::REGX
        .elseif .xmatch(.mid(0,_pos_,{exp}),y) ; reg y
            _left_ .set _COMP_MACRO_::REGY
        .elseif .xmatch(.mid(0,_pos_,{exp}),a) ; reg a
            _left_ .set _COMP_MACRO_::REGA
        .endif
    .endif

    .if _left_ = _COMP_MACRO_::INVALID ; no registers found, so first we will load a register
        lda .mid(0,_pos_,{exp})
        _left_ .set _COMP_MACRO_::REGA
        .out "WARNING: Reg a value assigned in signed comparison macro."
    .endif

    .if _op_ > 2 ; EQUAL or NOTEQUAL

        .if _left_ = _COMP_MACRO_::REGX
            cpx .mid(_right_pos_,_right_pos_count_, {exp})
        .elseif _left_ = _COMP_MACRO_::REGY
            cpy .mid(_right_pos_,_right_pos_count_, {exp})
        .elseif _left_ = _COMP_MACRO_::REGA
            cmp .mid(_right_pos_,_right_pos_count_, {exp})
        .endif

        .if _op_ = _COMP_MACRO_::EQUAL
            set_flag_test Z set
        .elseif _op_= _COMP_MACRO_::NOTEQUAL
            set_flag_test Z clear
        .endif

    .else
        .if ___HL::__KEEP_REG_A_
            pha
        .else
            .out "WARNING: Reg a value changed in signed comparison macro."
        .endif

        .if _left_ = _COMP_MACRO_::REGX
            txa
        .elseif _left_ = _COMP_MACRO_::REGY
            tya
        .endif

        .if (_op_ = _COMP_MACRO_::LESS) .or  (_op_ = _COMP_MACRO_::GREATEREQUAL)                                                ; less or greaterorequal
            sec                       ; prepare carry for SBC
            sbc .mid(_right_pos_,_right_pos_count_, {exp})   ; A-NUM

            .if ___HL::__KEEP_REG_A_
                bvs jmp_over     ; if V is 0, N eor V = N, otherwise N eor V = N eor 1
            .else
                bvc jmp_over
            .endif

            eor #$80          ; A = A eor $80, and N = N eor 1
            jmp_over:
            ; If the N flag is 1, then A (signed) < NUM (signed) and BMI will branch
            ; If the N flag is 0, then A (signed) >= NUM (signed) and BPL will branch
            .if ___HL::__KEEP_REG_A_
                asl
                .if _op_ = _COMP_MACRO_::LESS
                    set_flag_test C clear
                .elseif _op_= _COMP_MACRO_::GREATEREQUAL
                    set_flag_test C set
                .endif

            .else

                .if _op_ = _COMP_MACRO_::LESS
                    set_flag_test N set
                .elseif _op_= _COMP_MACRO_::GREATEREQUAL
                    set_flag_test N clear
                .endif

            .endif
        .endif

        .if (_op_ = _COMP_MACRO_::GREATER) .or  (_op_ = _COMP_MACRO_::LESSEQUAL)
            clc       ; Note: CLC, not SEC
            sbc .mid(_right_pos_,_right_pos_count_, {exp})

            .if ___HL::__KEEP_REG_A_
                bvs jmp_over     ; if V is 0, N eor V = N, otherwise N eor V = N eor 1
            .else
                bvc jmp_over
            .endif


            eor #$80
            jmp_over2:
            ; If the N flag is 1, then A (signed) <= NUM (signed) and BMI will branch
            ; If the N flag is 0, then A (signed) > NUM (signed) and BPL will branch

            .if ___HL::__KEEP_REG_A_
                asl
                .if _op_ = _COMP_MACRO_::GREATER
                    set_flag_test C set
                .elseif _op_= _COMP_MACRO_::LESSEQUAL
                    set_flag_test C clear
                .endif
            .else
                .if _op_ = _COMP_MACRO_::GREATER
                    set_flag_test N clear
                .elseif _op_= _COMP_MACRO_::LESSEQUAL
                    set_flag_test N set
                .endif
            .endif

        .endif

        .if ___HL::__KEEP_REG_A_
            pla
        .endif

    .endif

.endmacro

;*************************************************************************************************************************

.scope _MB_MACRO_

    NO_OP  = 0
    ADD    = 1
    SUB    = 2
    ADDC   = 3
    SUBC   = 4
    TIMES  = 5
    DIVIDE = 6
    SHL    = 7
    SHR    = 8
    _AND   = 9
    _OR    = 10
    _EOR   = 11

    INVALID = 0
    REGX    = 1
    REGY    = 2
    REGA    = 3

    rightside_reg .set 0
    Last_reg_used .set 0

.endscope

.scope _POWERS_
POWEROF2_128 = 7
POWEROF2_64 = 6
POWEROF2_32 = 5
POWEROF2_16 = 4
POWEROF2_8 = 3
POWEROF2_4 = 2
POWEROF2_2 = 1
.endscope

.macro find_mathlogic_operator exp, _pos_, _op_

    .if .xmatch(.mid(_pos_,1,{exp}),{(})
        _pos_ .set _pos_ + 1 ; skip over found bracket
        findposition {exp}, {)}, _pos_,(,),[,]
        .if .not _pos_
            .error ") Expected."
        .endif
    .elseif .xmatch(.mid(_pos_,1,{exp}),[)
        _pos_ .set _pos_ + 1
        findposition {exp}, ], _pos_,(,),[,]
        .if .not _pos_
            .error "] Expected."
        .endif

    .elseif .xmatch(.mid(_pos_,1,{exp}),+)      ; +
    
        .if .xmatch(.mid(_pos_+1,1,{exp}),+)     ; ++ ; if match this, do nothing, it is for label++scope type ident
            _pos_ .set _pos_ + 1
        .elseif .xmatch(.mid(_pos_+1,1,{exp}),c) || .xmatch(.mid(_pos_+1,1,{exp}),C) ; +c or +C
            _op_ .set _MB_MACRO_::ADDC
            _pos_ .set _pos_ + 1
        .else ; no match ++, and no match +c or +C, must just be a +
            _op_ .set _MB_MACRO_::ADD
        .endif
        
    .elseif .xmatch(.mid(_pos_,1,{exp}),-)
        .if .xmatch(.mid(_pos_+1,1,{exp}),c) || .xmatch(.mid(_pos_+1,1,{exp}),C) ; +c or +C
            _op_ .set _MB_MACRO_::SUBC
            _pos_ .set _pos_ + 1
        .else
            _op_ .set _MB_MACRO_::SUB
        .endif
    .elseif .xmatch(.mid(_pos_,1,{exp}),*)
        _op_ .set _MB_MACRO_::TIMES
    .elseif  .xmatch(.mid(_pos_,1,{exp}),/)
        _op_ .set _MB_MACRO_::DIVIDE
    .elseif  .xmatch(.mid(_pos_,1,{exp}),>>)
        _op_ .set _MB_MACRO_::SHR
    .elseif  .xmatch(.mid(_pos_,1,{exp}),<<)
        _op_ .set _MB_MACRO_::SHL
    .elseif  .xmatch(.mid(_pos_,1,{exp}),&)
        _op_ .set _MB_MACRO_::_AND
    .elseif .xmatch(.mid(_pos_,1,{exp}),|)
        _op_ .set _MB_MACRO_::_OR
    .elseif .xmatch(.mid(_pos_,1,{exp}),^)
        _op_ .set _MB_MACRO_::_EOR
    .endif

    .if _pos_ < .tcount({exp})
        .if .not _op_
            _pos_ .set _pos_ + 1
            find_mathlogic_operator {exp}, _pos_, _op_
        .endif
    .else
        .if .not _op_
            _pos_ .set 0
        .endif
    .endif

.endmacro

; Move a byte value and/or perform an operation(s) on it

.macro mb exp, exprecursive, noassigment, found_pos, found_op ; first time 'exp' could be a forced register

.local _equalpos_, _op_, _pos_, _user_forced_reg_


    _equalpos_  .set 0
    _op_        .set 0
    _pos_       .set 0

    ; figure out which and how to use user specified regsiter to eval right side expression
    .ifnblank exprecursive
        ; here: register was specified, try to force it:
        .if .xmatch({exp}, a)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA
        .elseif .xmatch({exp}, x)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGX
        .elseif .xmatch({exp}, y)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGY
        .else
            .error "Unknown register or bad comma in expression."
        .endif

        mb {exprecursive}
        .exitmacro
    .else

        ; deal with 'a:' token 1st
        findposition {exp}, a:, _equalpos_
        .if _equalpos_ ; will be -1 to indicate matched first token
            _equalpos_ .set _equalpos_ + (_equalpos_ = -1)
            .if .xmatch({.mid(_equalpos_+1,1,{exp})},{=})
                ; here:, we found "a:=" with no space, ca65 matches 'a:' as a token so fix it
                mb { a := .mid(_equalpos_+2,.tcount({exp})-_equalpos_-2,{exp}) }
                .exitmacro
            .endif
        .endif

        _user_forced_reg_ .set _MB_MACRO_::rightside_reg ; if set, user set it, save setting for forcereg or not

        findposition {exp}, :=, _equalpos_
        .if .not _equalpos_
            .ifblank noassigment ; internal hacky use for nice conditional expressions
                .error "No assignment found in byte assignment macro."
                .exitmacro
            .endif
            _equalpos_ .set -1 ; continue without equal
        .endif

        .if .not _MB_MACRO_::rightside_reg ; no manual register forced:

            ; see if register is specified on the right and if we can use it later
            .if .xmatch(.mid(_equalpos_+1, 1,{exp}),x)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGX
            .elseif .xmatch(.mid(_equalpos_+1, 1,{exp}),y)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGY
            .elseif .xmatch(.mid(_equalpos_+1, 1,{exp}),a)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA
            .endif

            .if _MB_MACRO_::rightside_reg ; if now set, count this as a sort of manual override setting
                _user_forced_reg_ .set _MB_MACRO_::rightside_reg ; save setting for forcereg or not

            ; now: check if left side is just a register, try to use that register on the right
            ; if the operator says we can't use it, we use _MB_MACRO_::REGA
            .elseif .xmatch(.mid(0, _equalpos_,{exp}),x)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGX
            .elseif .xmatch(.mid(0, _equalpos_,{exp}),y)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGY
            .elseif .xmatch(.mid(0, _equalpos_,{exp}),a)
                _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA
            .endif

        .endif


        ; look on the right of equal only
        .ifblank found_op
            find_mathlogic_operator {.mid(_equalpos_+1, .tcount({exp}) - (_equalpos_ + 1),{exp})}, _pos_, _op_ ; look for operator on right side of :=
            ; adjust:
            _pos_ .set _pos_ + _equalpos_ + 1
        .else ; we found it already, so use it - macro/instrution scanning will do this
            _pos_ .set found_pos
            _op_  .set found_op
        .endif

        ; if operator is + or - and we are using x or y do so with iny,dey,inx,dex

        .if ((_op_ = _MB_MACRO_::SUB) .or ( _op_ = _MB_MACRO_::ADD)) .and ((_MB_MACRO_::rightside_reg = _MB_MACRO_::REGY) .or (_MB_MACRO_::rightside_reg = _MB_MACRO_::REGX)) .and .match(.mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}),1) ; match a number

            .if _MB_MACRO_::rightside_reg = _MB_MACRO_::REGY

                ; if first token is not y, load something
                .if .not .xmatch(.mid(_equalpos_ + 1, 1,{exp}),y)
                    do_array_syntax ldy, {.mid(_equalpos_ + 1, _pos_ - _equalpos_ - 1 ,{exp})}
                .endif

                .if _op_ = _MB_MACRO_::SUB
                    .if .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}) > _MB_MAXIMUM_ADD_OR_SUB_X_OR_Y_
                        .warning "Created more than the maximum allowed dey commands."
                    .endif

                    .repeat .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp})
                        dey
                    .endrepeat

                .elseif _op_ = _MB_MACRO_::ADD

                    .if .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}) > _MB_MAXIMUM_ADD_OR_SUB_X_OR_Y_
                        .warning "Created more than the maximum allowed iny commands."
                    .endif
                    .repeat .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp})
                        iny
                    .endrepeat

                .endif


            .elseif _MB_MACRO_::rightside_reg = _MB_MACRO_::REGX

                ; if first token is not x, load something
                .if .not .xmatch(.mid(_equalpos_ + 1, 1,{exp}),x)
                    do_array_syntax ldx, {.mid(_equalpos_ + 1, _pos_ - _equalpos_ - 1 ,{exp})}
                .endif

                .if _op_ = _MB_MACRO_::SUB
                    .if .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}) > _MB_MAXIMUM_ADD_OR_SUB_X_OR_Y_
                        .warning "Created more than the maximum allowed dex commands."
                    .endif
                    .repeat .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp})
                        dex
                    .endrepeat

                .elseif _op_ = _MB_MACRO_::ADD
                    .if .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}) > _MB_MAXIMUM_ADD_OR_SUB_X_OR_Y_
                        .warning "Created more than the maximum allowed inx commands."
                    .endif
                    .repeat .mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp})
                        inx
                    .endrepeat

                .endif

            .endif

        .elseif _op_

            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA

            ; here, we use rega for all operations:

            ; here: do a LDA if the first token is not 'a'
            .if .not .xmatch(.mid(_equalpos_ + 1, 1,{exp}),a)
                .if _op_ = _MB_MACRO_::ADDC .or _op_ = _MB_MACRO_::SUBC ; adjust if the op is +c or -c
                    do_array_syntax lda, {.mid(_equalpos_ + 1, _pos_ - _equalpos_ - 2 ,{exp})}
                .else
                    do_array_syntax lda, {.mid(_equalpos_ + 1, _pos_ - _equalpos_ - 1 ,{exp})}
                .endif
            .endif

            _evalute_mathlogic_ {.mid(_pos_ + 1,.tcount({exp})-_pos_-1, {exp}) }, _op_ ; recursivley go through the operators, starting with 1st operand

        ; no operator: check if right side is just a register ; should be error check here is reg was manually set
        .elseif .xmatch(.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp}),a)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA
        .elseif .xmatch(.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp}),x)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGX
        .elseif .xmatch(.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp}),y)
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGY
        .elseif _MB_MACRO_::rightside_reg = _MB_MACRO_::REGX
            .if .not .xmatch(.mid(_equalpos_ + 1, 1,{exp}),x)
                do_array_syntax ldx, {.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp})}
            .endif
        .elseif _MB_MACRO_::rightside_reg = _MB_MACRO_::REGY
            .if .not .xmatch(.mid(_equalpos_ + 1, 1,{exp}),y)
                do_array_syntax ldy, {.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp})}
            .endif
        .else
            _MB_MACRO_::rightside_reg .set _MB_MACRO_::REGA ; must be _MB_MACRO_::REGA - default
            do_array_syntax lda, {.mid(_equalpos_ + 1, .tcount({exp}) - _equalpos_ - 1,{exp})}
        .endif

    .endif

    .if _user_forced_reg_ .and (_user_forced_reg_  <> _MB_MACRO_::rightside_reg)
        .error "Cannot evaluate this line with this register."
    .endif

    .ifblank noassigment ; if macro was called to eval right side only we don't do this:

        .if _MB_MACRO_::rightside_reg = _MB_MACRO_::REGA

            .if .xmatch(.mid(0,_equalpos_, {exp}),x)
                tax
            .elseif .xmatch(.mid(0,_equalpos_, {exp}),y)
                tay
            .elseif .not .xmatch(.mid(0,_equalpos_, {exp}),a)
                do_array_syntax sta, {.mid(0,_equalpos_, {exp})}
            .endif

        .elseif _MB_MACRO_::rightside_reg = _MB_MACRO_::REGX

            .if .xmatch(.mid(0,_equalpos_, {exp}),a)
                txa
            .elseif .xmatch(.mid(0,_equalpos_, {exp}),y)
                .error "Assigment of y := x not supported."
            .elseif .not .xmatch(.mid(0,_equalpos_, {exp}),x)
                do_array_syntax stx, {.mid(0,_equalpos_, {exp})}
            .endif

        .elseif _MB_MACRO_::rightside_reg = _MB_MACRO_::REGY

            .if .xmatch(.mid(0,_equalpos_, {exp}),a)
                tya
            .elseif .xmatch(.mid(0,_equalpos_, {exp}),x)
                .error "Assigment of x := y not supported."
            .elseif .not .xmatch(.mid(0,_equalpos_, {exp}),y)
                do_array_syntax sty, {.mid(0,_equalpos_, {exp})}
            .endif
        .endif
    .endif

    _MB_MACRO_::Last_reg_used .set _MB_MACRO_::rightside_reg ; for other code to check (like comp)
    _MB_MACRO_::rightside_reg .set 0

.endmacro

.macro _evalute_mathlogic_ exp, _op_

; this macro expects to have an _op_eration set and looks for any remaining:


.local _pos2_,_op2_

    _pos2_ .set 0
    _op2_    .set 0
    ; look for 2nd operator, first operator is stripped, is in '_op_'
    find_mathlogic_operator {exp}, _pos2_, _op2_

    .if _pos2_
 
        .if _op2_ = _MB_MACRO_::ADDC .or _op2_ = _MB_MACRO_::SUBC
            _evalute_mathlogic_ {.mid(0, _pos2_ - 1, {exp})},_op_ ; do first part, adjust for +c, -c
        .else
            _evalute_mathlogic_ {.mid(0, _pos2_, {exp})},_op_ ; do first part, no more ops here
        .endif
        _evalute_mathlogic_ {.mid(_pos2_+1, .tcount({exp}) - _pos2_ - 1, {exp})},_op2_ ; this can have more than one op
        ; .out .string(.mid(_pos_+1,1,{exp}))
        .exitmacro
    .endif

    .if _op_ = _MB_MACRO_::ADDC
        .if .blank(exp)
            adc #0
        .else
            do_array_syntax adc, {exp}
        .endif
    .elseif _op_ = _MB_MACRO_::SUBC
        .if .blank(exp)
            sbc #0
        .else
            do_array_syntax sbc, {exp}
        .endif
    .elseif _op_ = _MB_MACRO_::SUB
        sec
        do_array_syntax sbc, {exp}
    .elseif _op_ = _MB_MACRO_::ADD
        clc
        do_array_syntax adc, {exp}
    .elseif _op_ = _MB_MACRO_::TIMES
        .repeat _POWERS_::.ident(.sprintf("POWEROF2_%d",exp))
            asl a
        .endrepeat
    .elseif _op_ = _MB_MACRO_::DIVIDE
        .repeat _POWERS_::.ident(.sprintf("POWEROF2_%d",exp))
            lsr a
        .endrepeat
    .elseif _op_ = _MB_MACRO_::SHL
        .repeat exp
            asl a
        .endrepeat
    .elseif _op_ = _MB_MACRO_::SHR
        .repeat exp
            lsr a
        .endrepeat
    .elseif _op_ = _MB_MACRO_::_AND
        do_array_syntax and, {exp}
    .elseif _op_ = _MB_MACRO_::_OR
        do_array_syntax ora, {exp}
    .elseif _op_ = _MB_MACRO_::_EOR
        do_array_syntax eor, {exp}
    .endif

.endmacro

.macro do_array_syntax I, exp, forceindirect

; accept syntax with square brackets instead of "+ constant" or ",x" or ",y"
; can accept any combination of constant expression inside square brackets as well as one 'x' or 'y' either at the start  '[x...'
; or inside the experssion as ..+x..

.local _left_pos_, _right_pos_, _reg_pos_

    _left_pos_ .set 0
    _right_pos_ .set 0
    _reg_pos_ .set 0

    ; findposition {exp},{.}, _left_pos_,(,) ; find first [ bracket
    ; .if _left_pos_

    findposition {exp},[, _left_pos_,(,) ; find first [ bracket

    .if .not _left_pos_
        ; no square brackets:
        ; here we check if () actually indicate an indirect.
        ; only jmp has plain indirect, everything else has a ,x or ,y somewhere
        ; check for () brackets and remove them if this is not indirect, since ca65 doesn't like them on the otside of the expression.
        checkbrackets {exp}
        .if ___HL::_AND_POS_ = -1 ; indicates we matched outside bracket over whole operand

            findposition {exp}, {,} , _left_pos_ ; look inside brackets for ','
            .if .xmatch(.mid(_left_pos_+1,1,{exp}),x) .or .xmatch(.mid(_left_pos_+1,1,{exp}),y) ; if comma, and next character is a x or y then call as is, let ca65 deal with bad syntax
                I exp
            .else
                findposition {exp}, [, _left_pos_ ; look inside brackets for '['
                .if _left_pos_
                    findposition {exp}, x, _left_pos_ ; look inside brackets for 'x', if so try it with indirect option
                    .if _left_pos_
                        do_array_syntax I, {.mid(1,___HL::_OR_POS_ -1,{exp})}, indirect ; recurse, but activate indirect mode
                        .exitmacro
                    .endif
                .endif
                codeout I, {.mid(1,___HL::_OR_POS_ -1,{exp})} ; normal instruction with brackets stripped (not 65c02 compatible)
                .exitmacro
            .endif

        .else
            I exp ; no brackets on outside of expression -> normal code
            .exitmacro
        .endif
    .else
        findposition {exp},], _right_pos_,(,)
        .if _left_pos_ > _right_pos_
            .error "Array index syntax error."
        .endif
    .endif
    ; look inside square brackets:

    _reg_pos_ .set _left_pos_
    findposition {exp},x, _reg_pos_,(,) ; look for x
    .if _reg_pos_
        .define _which_reg_in_array x
    .else
        _reg_pos_ .set _left_pos_
        findposition {exp},y, _reg_pos_,(,) ; look for y
        .if _reg_pos_
            .define _which_reg_in_array y
        .endif
    .endif


    .if _reg_pos_
        .if _left_pos_ + 1 = _reg_pos_ ; pattern -> '[x....

            .if _right_pos_ > _reg_pos_ + 1 ; if more than just an x inside:

                codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp}).mid(_reg_pos_+1, _right_pos_ - _reg_pos_ - 1,{exp}),_which_reg_in_array}, forceindirect

            .else ; just an [x]
                codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp}),_which_reg_in_array}, forceindirect
            .endif

        .else ; x is somewhere in the brackets, remove it
            .if .not .xmatch(.mid(_reg_pos_-1,1,{exp}),+)  ; x must have a plus to the left, nothing else is allowed
                .error "Invalid operator in array expression.(Only addition of index register allowed.)"
            .endif
            .if _reg_pos_ + 1 = _right_pos_ ; ...+x]
                .if .xmatch({.mid(_left_pos_+1, 1,{exp})},-)
                    codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})-.mid(_left_pos_+2, _right_pos_ - _left_pos_ - 4,{exp}),_which_reg_in_array}, forceindirect
                .else
                    codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})+.mid(_left_pos_+1, _right_pos_ - _left_pos_ - 3,{exp}),_which_reg_in_array}, forceindirect
                .endif
            .else
                .if .xmatch({.mid(_left_pos_+1, 1,{exp})},-)
                    codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})-.mid(_left_pos_+2, _reg_pos_ - _left_pos_ - 3,{exp}).mid(_reg_pos_+1, _right_pos_ - _reg_pos_ - 1,{exp}),_which_reg_in_array}, forceindirect
                .else
                    codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})+.mid(_left_pos_+1, _reg_pos_ - _left_pos_ - 2,{exp}).mid(_reg_pos_+1, _right_pos_ - _reg_pos_ - 1,{exp}),_which_reg_in_array}, forceindirect
                .endif
            .endif
        .endif
        .undefine _which_reg_in_array
    .else ; no x or y, just a constant:
        .if .xmatch({.mid(_left_pos_+1, 1,{exp})},-)
            codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})-.mid(_left_pos_ + 1, _right_pos_ - _left_pos_ - 1, {exp})}, forceindirect
        .else
            codeout I, {.mid(0, _left_pos_,{exp}).mid(_right_pos_ + 1, .tcount({exp}) - 1,{exp})+.mid(_left_pos_ + 1, _right_pos_ - _left_pos_ - 1, {exp})}, forceindirect
        .endif
    .endif

.endmacro

; not used or integrated (yet):

.macro mw exp
    _equalpos_ .set 1 ; first check at token 1 since there should be something on the left
    findposition exp, :=, _equalpos_

    .if (_equalpos_ = 0)
        .error "No assignment found in word assignment macro."
        .exitmacro
    .endif

    .if  .xmatch(.mid((_equalpos_+1),1, {exp}), #) ; immeidate mode

        lda  #<.mid((_equalpos_+2), .tcount({exp}) - _equalpos_  - 2 , {exp})
        sta .mid(0,_equalpos_,{exp})

        lda  #>.mid((_equalpos_+2), .tcount({exp}) - _equalpos_  - 2 , {exp})

        sta .mid(0,_equalpos_,{exp}) + 1

    .else

        lda  .mid((_equalpos_+1), .tcount({exp}) - _equalpos_  - 1 , {exp})
        sta .mid(0,_equalpos_,{exp})
        lda  .mid((_equalpos_+1), .tcount({exp}) - _equalpos_  - 1 , {exp})+1
        sta .mid(0,_equalpos_,{exp}) + 1

    .endif
.endmacro

; label+scope wrapper:


.macro makelabel ident_, label_, scope_

    .define ident_ label_++scope_ ; three plus seems to work and can be scanned as an ignore in other HL macros

.endmacro

.macro mystruct name
    .define mystruct_name name
    
    .ident(.sprintf("__struct_label__%s_",.string(mystruct_name))):
    .struct .ident(.sprintf("__struct_scope__%s_",.string(mystruct_name)))
.endmacro

    
.macro endmystruct

    .endstruct
    .res .sizeof(.ident(.sprintf("__struct_scope__%s_",.string(mystruct_name))))
    
    makelabel mystruct_name, .ident(.sprintf("__struct_label__%s_",.string(mystruct_name))),.ident(.sprintf("__struct_scope__%s_",.string(mystruct_name)))
    .undefine mystruct_name
.endmacro
