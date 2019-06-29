#include "source/graphics/static/custom_title.h"
#include "source/graphics/static/custom_title_assets.h"
#include "source/configuration/game_states.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"

#define FATAL_MODE 2
#define NORMAL_MODE 1

void flash_start_text(unsigned char interval)
{
    if (tempChar2 > interval)
    {
        if (tempChar1 == 1)
        {
            pal_col(11, 0x26);
            tempChar1 = 0;
        }
        else
        {
            pal_col(11, 0x0f);
            tempChar1 = 1;
        }
        tempChar2 = 0;
    }
    ++tempChar2;
}

void handle_area_selection_input(void)
{
    if (tempChar1 == FATAL_MODE)
    { 
        tempChar2 = rand8() % 10 - 5;
        tempChar3 = rand8() % 10 - 5;
        scroll(tempChar2, tempChar3);
    }
        
    //   ++tempChar5;
    //   oam_spr(tempChar5,tempChar5, 0x77, 0,tempChar4); 
}
void draw_area_select(void)
{
    ppu_off();
    vram_inc(0);
    vram_adr(NAMETABLE_A);
    pal_bg(area_select_normal_palette);
    pal_spr(area_select_normal_palette);
    vram_unrle(area_select_normal_rle);
    ppu_on_all();
    tempChar1 = NORMAL_MODE;
}

void draw_area_select_fatal(void)
{
    ppu_off();
    vram_inc(0);
    vram_adr(NAMETABLE_A);
    pal_bg(area_select_fatal_palette);
    pal_spr(area_select_fatal_palette);
    vram_unrle(area_select_fatal_rle);
    ppu_on_all();
    tempChar1 = FATAL_MODE;
}
void draw_custom_title(void)
{
    ppu_off();

    // Select our set of chr tiles
    set_chr_bank_0(CHR_BANK_CUSTOM_TITLE);
    set_chr_bank_1(CHR_BANK_CUSTOM_TITLE);

    // Remove any stray sprites
    oam_clear();

    // Set the palettes
    pal_bg(custom_title_palette);
    pal_spr(custom_title_palette);

    // Load up the nametable, and uncompress it onto the screen.
    vram_unrle(custom_title_nametable_rle);

    // We're done; show it to the user and move on.
    ppu_on_all();
    gameState = GAME_STATE_TITLE_INPUT;
}

void handle_custom_title_input(void)
{
    if (pad_trigger(0) & PAD_START)
    {
        gameState = GAME_STATE_START_PRESSED;
        tempChar1 = 0;
        tempChar2 = 0;
    }
}