; Memory layout for smbdis

; Consants 

;  GAME SPECIFIC DEFINES

    ; -------------------------------------------------------------------------------------------------------
    ; ----------------------------Constants------------------------------------------------------------------
    ; -------------------------------------------------------------------------------------------------------    
    
    ; sound effects constants

SFX_SmallJump         = %10000000
SFX_Flagpole          = %01000000
SFX_Fireball          = %00100000
SFX_PipeDown_Injury   = %00010000
SFX_EnemySmack        = %00001000
SFX_EnemyStomp        = %00000100
SFX_Bump              = %00000010
SFX_BigJump           = %00000001

SFX_BowserFall        = %10000000
SFX_ExtraLife         = %01000000
SFX_PowerUpGrab       = %00100000
SFX_TimerTick         = %00010000
SFX_Blast             = %00001000
SFX_GrowVine          = %00000100
SFX_GrowPowerUp       = %00000010
SFX_CoinGrab          = %00000001

SFX_BowserFlame       = %00000010
SFX_BrickShatter      = %00000001
    ; music constants

MUSIC_Silence          = %10000000
MUSIC_StarPower        = %01000000
MUSIC_PipeIntro        = %00100000
MUSIC_Cloud            = %00010000
MUSIC_Castle           = %00001000
MUSIC_Underground      = %00000100
MUSIC_Water            = %00000010
MUSIC_Ground           = %00000001
MUSIC_TimeRunningOut   = %01000000
MUSIC_EndOfLevel       = %00100000
MUSIC_AltGameOver      = %00010000
MUSIC_EndOfCastle      = %00001000
MUSIC_Victory          = %00000100
MUSIC_GameOver         = %00000010
MUSIC_Death            = %00000001

    ; enemy object constants

OBJECTID_GreenKoopa            = $00
OBJECTID_BuzzyBeetle           = $02
OBJECTID_RedKoopa              = $03
OBJECTID_HammerBro             = $05
OBJECTID_Goomba                = $06
OBJECTID_Bloober               = $07
OBJECTID_BulletBill_FrenzyVar  = $08
OBJECTID_TallEnemy             = $09
OBJECTID_GreyCheepCheep        = $0a
OBJECTID_RedCheepCheep         = $0b
OBJECTID_Podoboo               = $0c
OBJECTID_PiranhaPlant          = $0d
OBJECTID_GreenParatroopaJump   = $0e
OBJECTID_RedParatroopa         = $0f
OBJECTID_GreenParatroopaFly    = $10
OBJECTID_Lakitu                = $11
OBJECTID_Spiny                 = $12
OBJECTID_FlyingCheepCheep      = $14
OBJECTID_FlyCheepCheepFrenzy   = $14
OBJECTID_BowserFlame           = $15
OBJECTID_Fireworks             = $16
OBJECTID_BBill_CCheep_Frenzy   = $17
OBJECTID_Stop_Frenzy           = $18
OBJECTID_Bowser                = $2d
OBJECTID_PowerUpObject         = $2e
OBJECTID_VineObject            = $2f
OBJECTID_FlagpoleFlagObject    = $30
OBJECTID_StarFlagObject        = $31
OBJECTID_JumpspringObject      = $32
OBJECTID_BulletBill_CannonVar  = $33
OBJECTID_RetainerObject        = $35

; other constants

.enum
    WORLD1
    WORLD2
    WORLD3
    WORLD4
    WORLD5
    WORLD6
    WORLD7
    WORLD8
.endenum

.enum
    LEVEL1
    LEVEL2
    LEVEL3
    LEVEL4
.endenum

.enum
    MODE_TITLESCREEN
    MODE_GAMEPLAY
    MODE_VICTORY
    MODE_GAMEOVER
.endenum
    
    ; additional HL conditional defines

.define BigMario Z set        ; \ for use with ' PlayerSize '
.define SmallMario Z clear    ; / 

WarmBootOffset        = <_WarmBootOffset
ColdBootOffset        = <_ColdBootOffset

TitleScreenDataOffset = $1ec0                     ; address in CHR ROM for titlescreen nametable layout

    ; -------------------------------------------------------------------------------------------------------
    ; ------------------------------R -------A --------M ----------------------------------------------------
    ; -------------------------------------------------------------------------------------------------------

.segment "ZEROPAGE"

temp_byte:                      .res  8 ; local  temp pointers and vars
ObjectOffset:                   .res  1
FrameCounter:                   .res  1
A_B_Buttons:                    .res  1
Up_Down_Buttons:                .res  1
Left_Right_Buttons:             .res  1
PreviousA_B_Buttons:            .res  1
GameEngineSubroutine:           .res  1
Enemy_Flag:                     .res  7
Enemy_ID :                      .res  7

;--------------------------- Obj State:

Player_State:                   .res  1
; on ground = 0
; swimming = 1
; falling = 2
; ClimbmingVine = 3

Enemy_State:                    .res  6
Fireball_State:                 .res  2
Block_State:                    .res  4
Misc_State:                     .res  9

;--------------------------- 

PlayerFacingDir:                .res  1 ; 1 = right, 2 = left
FirebarSpinDirection:
DestinationPageLoc:             .res  1
VictoryWalkControl:             .res  4
PowerUpType:                    .res  1 ; 0 = shroom, 1 = fireflower, 2 = star, 3 = 1up shroom
FireballBouncingFlag:           .res  2
HammerBroJumpTimer:             .res  9
Player_MovingDir:               .res  1
Enemy_MovingDir:                .res  17


;--------------------------- X speed
SprObject_X_Speed:

Player_X_Speed:                 .res  1

Enemy_X_Speed:
LakituMoveSpeed:
PiranhaPlant_Y_Speed:
ExplosionGfxCounter:
Jumpspring_FixedYPos:
RedPTroopaCenterYPos:
BlooperMoveSpeed:
XMoveSecondaryCounter:
CheepCheepMoveMFlag:
FirebarSpinState_Low:
YPlatformCenterYPos:            .res 6

Fireball_X_Speed:               .res 2
Block_X_Speed:                  .res 4
Misc_X_Speed:                   .res 9

;--------------------------- Obj Page Loc:

SprObject_PageLoc:
Player_PageLoc:                 .res  1

Enemy_PageLoc:                  .res  6
Fireball_PageLoc:               .res  2
Block_PageLoc:                  .res  4
Misc_PageLoc:                   .res  9
Bubble_PageLoc:                 .res  3

;--------------------------- X position

SprObject_X_Position:
Player_X_Position:              .res  1

Enemy_X_Position:               .res  6
Fireball_X_Position:            .res  2
Block_X_Position:               .res  4
Misc_X_Position:                .res  9
Bubble_X_Position:              .res  3

;--------------------------- Y Speed

Player_Y_Speed:
SprObject_Y_Speed:              .res  1

FirebarSpinState_High:
XMovePrimaryCounter:
BlooperMoveCounter:
Enemy_Y_Speed:
LakituMoveDirection:
ExplosionTimerCounter:
PiranhaPlant_MoveFlag:          .res  6

Fireball_Y_Speed:               .res  2
Block_Y_Speed:                  .res  4
Misc_Y_Speed:                   .res  9

;--------------------------- Y high pos

SprObject_Y_HighPos:
Player_Y_HighPos:               .res  1

Enemy_Y_HighPos:                .res  6
Fireball_Y_HighPos:             .res  2
Block_Y_HighPos:                .res  4
Misc_Y_HighPos:                 .res  9
Bubble_Y_HighPos:               .res  3


;--------------------------- Y position

Player_Y_Position:
SprObject_Y_Position:           .res  1

Enemy_Y_Position:               .res  6
Fireball_Y_Position:            .res  2
Block_Y_Position:               .res  4
Misc_Y_Position:                .res  9
Bubble_Y_Position:              .res  3

;--------------------------- 

AreaData:
AreaDataLow:                    .res  1
AreaDataHigh:                   .res  1

EnemyData:
EnemyDataLow:                   .res  1
EnemyDataHigh:                  .res  1

dummylabel:                     .res 5 ; = start at $EB, reserve 5 bytes for local use.

NoteLenLookupTblOfs:            .res  1
Square1SoundBuffer:             .res  1
Square2SoundBuffer:             .res  1
NoiseSoundBuffer:               .res  1
AreaMusicBuffer:                .res  1

MusicData:
MusicDataLow:                   .res  1

MusicDataHigh:                  .res  1
MusicOffset_Square2:            .res  1
MusicOffset_Square1:            .res  1
MusicOffset_Triangle:           .res  1
PauseSoundQueue:                .res  1
AreaMusicQueue:                 .res  1
EventMusicQueue:                .res  1
NoiseSoundQueue:                .res  1
Square2SoundQueue:              .res  1
Square1SoundQueue:              .res  1



.segment "STACK"
; start $0100

blank_stack:                    .res  9 ; not used
VerticalFlipFlag:               .res  4
FlagpoleFNum_Y_Pos:             .res  1
FlagpoleFNum_YMFDummy:          .res  1
FlagpoleScore:                  .res  1


mystruct FloateyNum
    Control             .byte 7
    X_Pos               .byte 7 
    Y_Pos               .byte 7
endmystruct


ShellChainCounter:              .res  7
FloateyNum_Timer:               .res  8
DigitModifier:                  .res  6


.segment "OAMRAM"
; start $0200

mystruct Sprite
    .union
            Data            .byte
        .struct
            Y_Position      .byte
            Tilenumber      .byte
            Attributes      .byte
            X_Position      .byte
        .endstruct
    .endunion
endmystruct

.segment "BSS"
; start $0300

VRAM_Buffer1_Offset:            .res  1
VRAM_Buffer1:                   .res  63
VRAM_Buffer2_Offset:            .res  1
VRAM_Buffer2:                   .res  34 ; 26 for a column, 3 for address and size 

BowserBodyControls:             .res  1
BowserFeetCounter:              .res  1
BowserMovementSpeed:            .res  1
BowserOrigXPos:                 .res  1
BowserFlameTimerCtrl:           .res  1
BowserFront_Offset:             .res  1
BridgeCollapseOffset:           .res  1
BowserGfxFlag:                  .res  30

FirebarSpinSpeed:               .res  16

mystruct Vine

    FlagOffset              .byte
    Height                  .byte
    ObjOffset               .byte 3
    Start_Y_Position        .byte 3
    
endmystruct

BalPlatformAlignment:           .res  1
Platform_X_Scroll:              .res  1

HammerThrowingTimer:
PlatformCollisionFlag:          .res  11

Player_Rel_XPos:
SprObject_Rel_XPos:             .res  1

Enemy_Rel_XPos:                 .res  1
Fireball_Rel_XPos:              .res  1
Bubble_Rel_XPos:                .res  1
Block_Rel_XPos:                 .res  2
Misc_Rel_XPos:                  .res  5
SprObject_Rel_YPos:
Player_Rel_YPos:                .res  1
Enemy_Rel_YPos:                 .res  1
Fireball_Rel_YPos:              .res  1
Bubble_Rel_YPos:                .res  1
Block_Rel_YPos:                 .res  2
Misc_Rel_YPos:                  .res  6

Player_SprAttrib:
SprObject_SprAttrib:            .res  1

Enemy_SprAttrib:                .res  11

SprObject_OffscrBits:
Player_OffscreenBits:           .res  1

Enemy_OffscreenBits:            .res  1
FBall_OffscreenBits:            .res  1
Bubble_OffscreenBits:           .res  1
Block_OffscreenBits:            .res  2
Misc_OffscreenBits:             .res  2
EnemyOffscrBitsMasked:          .res  12
Block_Orig_YPos:                .res  2
Block_BBuf_Low:                 .res  2
Block_Metatile:                 .res  2
Block_PageLoc2:                 .res  2
Block_RepFlag:                  .res  2
SprDataOffset_Ctrl:             .res  2
Block_ResidualCounter:          .res  1
Block_Orig_XPos:                .res  8
AttributeBuffer:                .res  7
SprObject_X_MoveForce:          .res  1

Enemy_X_MoveForce:
YPlatformTopYPos:
RedPTroopaOrigXPos:             .res  21

SprObject_YMoveForceFractional:
Player_YMoveForceFractional:    .res  1

Enemy_YMoveForceFractional:
BowserFlamePRandomOfs:
PiranhaPlantUpYPos:             .res  21

Bubble_YMoveForceFractional:    .res  7

Player_Y_MoveForce:
SprObject_Y_MoveForce:          .res  1

CheepCheepOrigYPos:
Enemy_Y_MoveForce:
PiranhaPlantDownYPos:           .res  8

Block_Y_MoveForce:              .res  20
MaximumLeftSpeed:               .res  6
MaximumRightSpeed:              .res  20

Whirlpool_Offset:
Cannon_Offset:                  .res  1

Whirlpool_PageLoc:
Cannon_PageLoc:                 .res  6

Cannon_X_Position:
Whirlpool_LeftExtent:           .res  6

Whirlpool_Length:
Cannon_Y_Position:              .res  6

Cannon_Timer:
Whirlpool_Flag:                 .res  6

BowserHitPoints:                .res  1
StompChainCounter:              .res  12
Player_CollisionBits:           .res  1
Enemy_CollisionBits:            .res  8

Player_BoundBoxCtrl:
SprObj_BoundBoxCtrl:            .res  1

Enemy_BoundBoxCtrl:             .res  6
Fireball_BoundBoxCtrl:          .res  2
Misc_BoundBoxCtrl:              .res  10

BoundingBox_UL_XPos:
BoundingBox_UL_Corner:          .res  1

BoundingBox_UL_YPos:            .res  1

BoundingBox_DR_XPos:
BoundingBox_LR_Corner:          .res  1

BoundingBox_DR_YPos:            .res  1
EnemyBoundingBoxCoord:          .res  80
Block_Buffer_1:                 .res  208
Block_Buffer_2:                 .res  208
BlockBufferColumnPos:           .res  1
MetatileBuffer:                 .res  13
HammerEnemyOffset:              .res  9
JumpCoinMiscOffset:             .res  5
BrickCoinTimerFlag:             .res  2
Misc_Collision_Flag:            .res  13
EnemyFrenzyBuffer:              .res  1
SecondaryHardMode:              .res  1
EnemyFrenzyQueue:               .res  1
FireballCounter:                .res  1
DuplicateObj_Offset:            .res  2
LakituReappearTimer:            .res  2
NumberofGroupEnemies:           .res  1 ; only used in one subroutine , HandleGroupEnemies - sub could use a temp.
ColorRotateOffset:              .res  1
PlayerGfxOffset:                .res  1
WarpZoneControl:                .res  1
FireworksCounter:               .res  2
MultiLoopCorrectCntr:           .res  1
MultiLoopPassCntr:              .res  1
JumpspringForce:                .res  1
MaxRangeFromOrigin:             .res  1
BitMFilter:                     .res  1
ChangeAreaTimer:                .res  2
SprShuffleAmtOffset:            .res  1
SprShuffleAmt:                  .res  3

SprDataOffset:
Player_SprDataOffset:           .res  1

Enemy_SprDataOffset:            .res  7

Alt_SprDataOffset:
Block_SprDataOffset:            .res  2

Bubble_SprDataOffset:           .res  3
FBall_SprDataOffset:            .res  2

Misc_SprDataOffset:             .res  9

SavedJoypad1Bits:
SavedJoypadBits:                .res  1

SavedJoypad2Bits:               .res  2
Player_X_Scroll:                .res  1
Player_XSpeedAbsolute:          .res  1
FrictionAdderHigh:              .res  1
FrictionAdderLow:               .res  1
RunningSpeed:                   .res  1
SwimmingFlag:                   .res  1
Player_X_MoveForce:             .res  1
DiffToHaltJump:                 .res  1
JumpOrigin_Y_HighPos:           .res  1
JumpOrigin_Y_Position:          .res  1
VerticalForce:                  .res  1
VerticalForceDown:              .res  1
PlayerChangeSizeFlag:           .res  1
PlayerAnimTimerSet:             .res  1
PlayerAnimCtrl:                 .res  1
JumpspringAnimCtrl:             .res  1
FlagpoleCollisionYPos:          .res  1
PlayerEntranceCtrl:             .res  1
FireballThrowingTimer:          .res  1
DeathMusicLoaded:               .res  1
FlagpoleSoundQueue:             .res  1
CrouchingFlag:                  .res  1
GameTimerSetting:               .res  1
DisableCollisionDet:            .res  1
DemoAction:                     .res  1
DemoActionTimer:                .res  1
PrimaryMsgCounter:              .res  1

ScreenEdge_PageLoc:
ScreenLeft_PageLoc:             .res  1

ScreenRight_PageLoc:            .res  1

ScreenEdge_X_Pos:
ScreenLeft_X_Pos:               .res  1

ScreenRight_X_Pos:              .res  1
ColumnSets:                     .res  1
AreaParserTaskNum:              .res  1
CurrentNTAddr_High:             .res  1
CurrentNTAddr_Low:              .res  1
Sprite0HitDetectFlag:           .res  1
ScrollLock:                     .res  2
CurrentPageLoc:                 .res  1
CurrentColumnPos:               .res  1
TerrainControl:                 .res  1
BackloadingFlag:                .res  1
BehindAreaParserFlag:           .res  1
AreaObjectPageLoc:              .res  1
AreaObjectPageSel:              .res  1
AreaDataOffset:                 .res  1
AreaObjOffsetBuffer:            .res  3
AreaObjectLength:               .res  3
AreaStyle:                      .res  1
StaircaseControl:               .res  1
AreaObjectHeight:               .res  1
MushroomLedgeHalfLen:           .res  3
EnemyDataOffset:                .res  1
EnemyObjectPageLoc:             .res  1
EnemyObjectPageSel:             .res  1
ScreenRoutineTask:              .res  1
ScrollThirtyTwo:                .res  2
HorizontalScroll:               .res  1
VerticalScroll:                 .res  1
ForegroundScenery:              .res  1
BackgroundScenery:              .res  1
CloudTypeOverride:              .res  1
BackgroundColorCtrl:            .res  1
LoopCommand:                    .res  1
StarFlagTaskControl:            .res  1
TimerControl:                   .res  1 ; 0747
CoinTallyFor1Ups:               .res  1
SecondaryMsgCounter:            .res  1
JoypadBitMask:                  .res  4 ; .proc InitializeArea clears the 1st two bytes here and leaves below here alone

AreaType:                       .res  1 ; 074e

    ; notes:

    ; AreaType:
    ; Water = 0
    ; Ground = 1
    ; UnderGround = 2
    ; Castle = 3

AreaAddrsLOffset:               .res  1
AreaPointer:                    .res  1
EntrancePage:                   .res  1
AltEntranceControl:             .res  1
CurrentPlayer:                  .res  1 ; 0 = mario, 1 = luigi
PlayerSize:                     .res  1 ; 1 = small, 0 = big
Player_Pos_ForScroll:           .res  1
PlayerStatus:                   .res  1 ; 0 = small, 1 = super, 2 = firey
FetchNewGameTimerFlag:          .res  1
JoypadOverride:                 .res  1
GameTimerExpiredFlag:           .res  1

OnscreenPlayerInfo:
NumberofLives:                  .res  1

HalfwayPage:                    .res  1
LevelNumber:                    .res  1
Hidden1UpFlag:                  .res  1
CoinTally:                      .res  1
WorldNumber:                    .res  1
AreaNumber:                     .res  1

OffscreenPlayerInfo:
OffScr_NumberofLives:           .res  1

OffScr_HalfwayPage:             .res  1
OffScr_LevelNumber:             .res  1
OffScr_Hidden1UpFlag:           .res  1
OffScr_CoinTally:               .res  1
OffScr_WorldNumber:             .res  1
OffScr_AreaNumber:              .res  1
ScrollFractional:               .res  1
DisableIntermediate:            .res  1
PrimaryHardMode:                .res  1
WorldSelectNumber:              .res  5

; $0770: .proc InitializeGame leaves ram below here alone ( y = $6f )

OperMode:                       .res  2
OperMode_Task:                  .res  1
VRAM_Buffer_AddrCtrl:           .res  1
DisableScreenFlag:              .res  1
ScrollAmount:                   .res  1
GamePauseStatus:                .res  1
GamePauseTimer:                 .res  1
Mirror_PPU_CTRL:                .res  1
Mirror_PPU_MASK:                .res  1
NumberOfPlayers:                .res  5
IntervalTimerControl:           .res  1

Timers:
SelectTimer:                    .res  1
PlayerAnimTimer:                .res  1
JumpSwimTimer:                  .res  1
RunningTimer:                   .res  1
BlockBounceTimer:               .res  1
SideCollisionTimer:             .res  1
JumpspringTimer:                .res  1
GameTimerCtrlTimer:             .res  2
ClimbSideTimer:                 .res  1
EnemyFrameTimer:                .res  5
FrenzyEnemyTimer:               .res  1
BowserFireBreathTimer:          .res  1
StompTimer:                     .res  1
AirBubbleTimer:                 .res  3 ; 20 bytes away from Timer

ScrollIntervalTimer:            .res  1
EnemyIntervalTimer:             .res  7
BrickCoinTimer:                 .res  1
InjuryTimer:                    .res  1
StarInvincibleTimer:            .res  1
ScreenTimer:                    .res  1
WorldEndTimer:                  .res  1
DemoTimer:                      .res  5
PseudoRandomBitReg:             .res  9

SoundMemory:
MusicOffset_Noise:              .res  1
EventMusicBuffer:               .res  1
PauseSoundBuffer:               .res  1
Squ2_NoteLenBuffer:             .res  1
Squ2_NoteLenCounter:            .res  1
Squ2_EnvelopeDataCtrl:          .res  1
Squ1_NoteLenCounter:            .res  1
Squ1_EnvelopeDataCtrl:          .res  1
Tri_NoteLenBuffer:              .res  1
Tri_NoteLenCounter:             .res  1
Noise_BeatLenCounter:           .res  1
Squ1_SfxLenCounter:             .res  2
Squ2_SfxLenCounter:             .res  1
Sfx_SecondaryCounter:           .res  1
Noise_SfxLenCounter:            .res  1
DAC_Counter:                    .res  1
NoiseDataLoopbackOfs:           .res  3
NoteLengthTblAdder:             .res  1
AreaMusicBuffer_Alt:            .res  1

PauseModeFlag:                  .res  1
GroundMusicHeaderOfs:           .res  3
AltRegContentFlag:              .res  12

    _WarmBootOffset:            .res  1   ; Warm boot offset

TopScoreDisplay:                ; : $07D7
DisplayDigits:                  .res  6

PlayerScoreDisplay:
ScoreAndCoinDisplay:            .res  27

GameTimerDisplay:               .res  4
WorldSelectEnableFlag:          .res  1

ContinueWorld:                  .res  1

    _ColdBootOffset:            .res  1   ; Cold boot offset, here and higher get nuked
    
WarmBootValidation:             .res  1





