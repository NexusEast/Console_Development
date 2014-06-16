//this example code unpacks a RLE'd nametable into VRAM
//you can create the source data using NES Screen Tool

#include "neslib.h" 
#include "test_nam.h"	//packed nametable data
#include "map_data.h"
#define byte unsigned char
const unsigned char palette[16]={ 0x0f,0x02,0x05,0x0c,0x0f,0x02,0x1c,0x3d,0x0f,0x05,0x06,0x07,0x0f,0x30,0x30,0x16 };

#include <stdlib.h>


	//palette data

static unsigned char wait_time = 10;
static unsigned char temp_variable = 0;
static unsigned char game_status = 0; //0 == title screen.
static byte P1_JOY = 0;


#define NTADR(x,y) ((NAMETABLE_B|((y)<<5)|x)) 
#define MSB(x)		(((x)>>8))
#define LSB(x)		(((x)&0xff))
static unsigned char press_start[12*3+1];
const unsigned char press_start_init[12*3+1]={
	MSB(NTADR(8+2,25)),LSB(NTADR(8+2,25)),0xe5,//2
	MSB(NTADR(8+3,25)),LSB(NTADR(8+3,25)),0xe6,//5
	MSB(NTADR(8+4,25)),LSB(NTADR(8+4,25)),0xe9,//8
	MSB(NTADR(8+5,25)),LSB(NTADR(8+5,25)),0xe7,//11
	MSB(NTADR(8+6,25)),LSB(NTADR(8+6,25)),0xe7,//
	MSB(NTADR(8+7,25)),LSB(NTADR(8+7,25)),0xf7,//
	MSB(NTADR(8+8,25)),LSB(NTADR(8+8,25)),0xe7,//
	MSB(NTADR(8+9,25)),LSB(NTADR(8+9,25)),0xe8,//
	MSB(NTADR(8+10,25)),LSB(NTADR(8+10,25)),0xea,//
	MSB(NTADR(8+11,25)),LSB(NTADR(8+11,25)),0xe6,//
	MSB(NTADR(8+12,25)),LSB(NTADR(8+12,25)),0xe8,//
	NT_UPD_EOF
};
static byte frame_count = 0;




//put a string into the nametable

void put_str(unsigned int adr,const char *str)
{
	vram_adr(adr);

	while(1)
	{
		if(!*str) break;
		vram_put((*str++));//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void start_title_screen()
{

	delay(wait_time);
	++temp_variable;
	pal_bg_bright(temp_variable);
	if(temp_variable == 4 )++game_status;
}


static unsigned char meta_sprite_index = 0;

void press_start_flash()
{

	if(game_status == 2)
		++temp_variable;
	else
		temp_variable=0;
	if(temp_variable == 60)
	{
		temp_variable=4;
		++game_status;
	}

	if(frame_count% (game_status == 1?51:17 )  > (game_status == 1?25:9 ))
	{
		press_start[2] = press_start_init[2];
		press_start[5] = press_start_init[5];
		press_start[8] = press_start_init[8];
		press_start[11] = press_start_init[11];
		press_start[14] = press_start_init[14];
		press_start[17] = press_start_init[17];
		press_start[20] = press_start_init[20];
		press_start[23] = press_start_init[23];
		press_start[26] = press_start_init[26];
		press_start[29] = press_start_init[29];
		press_start[32] = press_start_init[32];
	}
	else 
	{
		press_start[2] = 0xf7;
		press_start[5] = 0xf7;
		press_start[8] = 0xf7;
		press_start[11] =0xf7;
		press_start[14] =0xf7;
		press_start[17] =0xf7;
		press_start[20] =0xf7;
		press_start[23] =0xf7;
		press_start[26] =0xf7;
		press_start[29] =0xf7;
		press_start[32] =0xf7;
	}

}
 
 void fade_to_game()
 {
 	delay(wait_time);
	--temp_variable;
	pal_bg_bright(temp_variable);
	if(temp_variable == 0 )
		{
			set_vram_update(NULL);
			pal_bg_bright(4);
			bankswitch(1);
			pal_bg(pal_beach);//SET UP BEACH SCENE
			ppu_off();
			vram_adr(NAMETABLE_A);//set VRAM address
			vram_unrle(name_table_beach);//set VRAM address
			ppu_on_all();
			++game_status;
		}
 }

void init_variables()
{
	//for title screen bright use
	temp_variable = 0;
	press_start[2] = 0xf7;
	press_start[5] = 0xf7;
	press_start[8] = 0xf7;
	press_start[11] =0xf7;
	press_start[14] =0xf7;
	press_start[17] =0xf7;
	press_start[20] =0xf7;
	press_start[23] =0xf7;
	press_start[26] =0xf7;
	press_start[29] =0xf7;
	press_start[32] =0xf7;
}
void main_game_fade_in()
{

}
void main(void)
{
	//rendering is disabled at the startup, and palette is all black
	
	pal_bg(palette);//set background palette from an array
	pal_bg_bright(0);
	vram_adr(NAMETABLE_A);//set VRAM address
	vram_unrle(test_nam);//unpack nametable into VRAM
 	pal_spr(palette);
	bankswitch(0);
 	     
	memcpy(press_start,press_start_init,sizeof(press_start_init));
	set_vram_update(press_start);
	init_variables();
	ppu_on_all();//enable rendering
	// put_str();
	while(1)
	{
		// game_status =3;
		P1_JOY = pad_trigger(0);
		++frame_count;
		switch (game_status)
		{
			case 0: //intro
				{
					start_title_screen();
					if(P1_JOY&PAD_START)
					{
						pal_bg_bright(4);
						++game_status;
					}
				}
			break;
			case 1: //press start flash
				{
					press_start_flash();

					if(P1_JOY&PAD_START)
					++game_status;
				}
			break;
			case 2: //start pressed
			{
					press_start_flash();
				}
				break;
			case 3: //fade to game
			{
					fade_to_game();
				}
			break;
			case 4: //main game-fade in
			{ 
				// scroll(0,frame_count);
				}
			break;
			default:
			break;
		}
		ppu_wait_nmi(); 
	}//do nothing, infinite loop
}


