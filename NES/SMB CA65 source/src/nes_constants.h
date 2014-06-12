; ******************************************************************************
; * ca65 NES_CONSTANTS.H



.ifndef _NES_CONSTANTS_H
_NES_CONSTANTS_H                        = 1

; General NES constant values

PPU_CTRL                               = $2000
PPU_MASK                               = $2001
PPU_STATUS                             = $2002
PPU_ADDRESS                            = $2006
PPU_DATA                               = $2007
OAM_ADDRESS                            = $2003
OAM_DATA                               = $2004
OAM_DMA                                = $4014
PPU_SCROLL                             = $2005

; ******************************************************************************
; * FLAGS/MASKS
; ******************************************************************************/

; For ppu_ctrl

CT_NMI                                 = %10000000
CT_PPUSLAVE                            = %01000000
CT_SPRITE8x16                          = %00100000
CT_BACKADDR1000                        = %00010000
CT_SPRADDR1000                         = %00001000
CT_ADDRINC32                           = %00000100
CT_NAMETABLE2000                       = %00000000
CT_NAMETABLE2400                       = %00000001
CT_NAMETABLE2800                       = %00000010
CT_NAMETABLE2C00                       = %00000011


; For ppu_mask

MA_PALETTE_BLUE                        = %10000000
MA_PALETTE_GREEN                       = %01000000
MA_PALETTE_RED                         = %00100000
MA_SPRITESVISIBLE                      = %00010000
MA_BACKVISIBLE                         = %00001000
MA_SPRNOCLIP                           = %00000100
MA_BACKNOCLIP                          = %00000010
MA_MONOCHROME                          = %00000001

; status flags ppu_status

ST_VBLANK                              = %10000000
ST_SPRITE0                             = %01000000
ST_SCANSPROVERFLOW                     = %00100000
ST_VRAM_NOWRITE                        = %00010000

; sprites

OAM_TOTAL                              = 64
OAM_SIZE                               = (4*OAM_TOTAL)

; palette

PAL_ADDRESS                            = $3F00
PAL_0_ADDRESS                          = $3F00
PAL_1_ADDRESS                          = $3F04
PAL_2_ADDRESS                          = $3F08
PAL_3_ADDRESS                          = $3F0C
PAL_0_SPR_ADDRESS                      = $3F10
PAL_1_SPR_ADDRESS                      = $3F14
PAL_2_SPR_ADDRESS                      = $3F18
PAL_3_SPR_ADDRESS                      = $3F1C

PAL_0                                  = $3F00 
PAL_1                                  = $3F04
PAL_2                                  = $3F08
PAL_3                                  = $3F0C
PAL_4                                  = $3F10
PAL_5                                  = $3F14
PAL_6                                  = $3F18
PAL_7                                  = $3F1C


; $3F01-$3F03     Background palette 0
; $3F05-$3F07     Background palette 1
; $3F09-$3F0B     Background palette 2
; $3F0D-$3F0F     Background palette 3
; $3F11-$3F13     Sprite palette 0
; $3F15-$3F17     Sprite palette 1
; $3F19-$3F1B     Sprite palette 2
; $3F1D-$3F1F     Sprite palette 3


; CHR

PATTERN_TABLE_0_ADDRESS                = $0000
PATTERN_TABLE_1_ADDRESS                = $1000

; NAME TABLE
NAMETABLE_WIDTH                        = 32
NAMETABLE_HEIGHT                       = 30
SCREEN_WIDTH                           = 256
SCREEN_HEIGHT                          = 240

NAME_TABLE_0_ADDRESS                   = $2000
NAME_TABLE_SIZE                        = $03C0
ATTRIBUTE_TABLE_0_ADDRESS              = $23C0
ATTRIBUTE_TABLE_SIZE                   = $0040
NAME_TABLE_1_ADDRESS                   = $2400
ATTRIBUTE_TABLE_1_ADDRESS              = $27C0
NAME_TABLE_2_ADDRESS                   = $2800
ATTRIBUTE_TABLE_2_ADDRESS              = $2BC0
NAME_TABLE_3_ADDRESS                   = $2C00
ATTRIBUTE_TABLE_3_ADDRESS              = $2FC0

; **********************************************************************
; Joystick
; **********************************************************************

 PAD0                                  = $4016
 PAD1                                  = $4017
 JOYPAD_PORT                           = $4016
 JOYPAD_PORT2                          = $4017

; joypad masks

BUTTON_A                              = %10000000
BUTTON_B                              = %01000000
BUTTON_SELECT                         = %00100000
BUTTON_START                          = %00010000
BUTTON_UP                             = %00001000
BUTTON_DOWN                           = %00000100
BUTTON_LEFT                           = %00000010
BUTTON_RIGHT                          = %00000001
 
; **********************************************************************
; Sound
; **********************************************************************

SND_REGISTER          = $4000
SND_SQUARE1_REG       = $4000
SND_SQUARE2_REG       = $4004
SND_TRIANGLE_REG      = $4008
SND_NOISE_REG         = $400c
SND_DELTA_REG         = $4010
SND_MASTERCTRL_REG    = $4015


.endif
