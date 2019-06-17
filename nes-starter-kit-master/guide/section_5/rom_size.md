# Resizing your rom

This guide gives you a generously sized ROM with room to grow, and a number of useful features. The engine is
custom-built to support this rom, but there are some tweaks we can make if the game isn't meeting your needs. This
chapter will walk you through doubling the size of your game, as well as the basics of the config files.

**If you make the changes described in this chapter, you will no longer be able to use the 128kb InfiniteNesLives  
cartridge linked in the "Playing your game on a real NES" chapter!** (Powerpak will continue to work) The 256kb
version will work with these tweaks, but you will have to look to make sure that the cartridge lines up with the
features you have added. (Especially if you do something custom)

This chapter does assume you understand the concept of prg banks - the first chapter in section 2 covers this. If you
are not, you may want to go there first.

## Doubling the size of your cartridge

So you've got a game going, and you are running out of space for your code. What can you do? Well, you can increase
the size avaialble, of course! From a code perspective this is somewhat simple - the only added costs would be in
buying hardware to make a physical cartridge. (And honestly, the cost there isn't that high either). Luckily, our
compiler knows how to do this, so we can get by with a configuration tweak alongside a small engine change.

First, we need to figure out what we want for space. (Okay, I know the title gives that away, but bear with me!) We 
currently have 128kb of program memory available to us, broken up into 8 16kb chunks. We won't go into explicit
detail here, but computer memory goes up in powers of two - like: `1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024`.
while it is possible to combine two smaller sizes, it is much easier to just go up to the next size. Since we
currently have 128kb of prg, we probably want to jump up to 256kb. This will give us 16 8kb banks in place of our 8.

This engine actually ships with a pre-built configuration file for this case. You can use it by updating `makefile`
to point at the new file. Look for this line: 

```makefile
CONFIG_FILE_PATH=tools/cc65_config/game
```

Change that to this to use the larger rom file configuration: 

```makefile
CONFIG_FILE_PATH=tools/cc65_config/game_256k
```

That should actually do it. Run a `make clean` to get rid of existing code, then build your game as normal to
get it up and running with its new size. You can run `make space_check` to verify. You can now put code into banks
7-15!

This will also allow you to use the 256kb 
[InfiniteNesLives SKROM board](http://www.infiniteneslives.com/nessupplies.php#MMC1), if you'd like.

If you want to understand this configuration file or make other tweaks, there is a section at the bottom of this file
that explains it in detail.

## Updating the configuration manually to resize your rom banks

**NOTE**: If you did the steps above to update your makefile, **you do not need to do this**. Just skip to the 
next chapter.  This is here for anyone who wants to understand the configuration file better, and make their 
own custom rom size that we don't natively support. 

The first thing we need to do is update the config file for the game. (If you are not using git, back up your
original file now! You will want a working configuration to go back to if things go wrong.) Open up
`tools/cc65_config/game.cfg`. This is gonna look a bit foreign, but it's pretty simple. Here's the parts we
care about: 

```
MEMORY {

    ZP: 		start = $0000, size = $0100, type = rw, define = yes;
    HEADER:		start = $0000, size = $0010, file = %O ,fill = yes;
	ROM_00:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
	ROM_01:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
	ROM_02:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
    ROM_03:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
	ROM_04:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
	ROM_05:		start = $8000, size = $4000, file = %O, fill = yes, define = no;
    ROM_06:     start = $8000, size = $4000, file = %O, fill = yes, define = no;
	PRG:		start = $c000, size = $3c00, file = %O, fill = yes, define = no;
    DMC: 		start = $fc00, size = $03f0, file = %O, fill = yes, define = yes;
    PRG_STUB:   start = $fff0, size = $0010, file = %O, fill = yes, define = no;
    CHR_00:		start = $0000, size = $1000, file = %O, fill = yes;

# ... a lot more CHR_XX entries, some other stuff...

}

SEGMENTS {

    HEADER:   load = HEADER,         type = ro;
    STARTUP:  load = PRG,            type = ro,  define = yes;
    LOWCODE:  load = PRG,            type = ro,                optional = yes;
    INIT:     load = PRG,            type = ro,  define = yes, optional = yes;
    ROM_00:		load = ROM_00,	type = ro, define = no;
	ROM_01:		load = ROM_01,	type = ro, define = no;
	ROM_02:		load = ROM_02,	type = ro, define = no;
    ROM_03:		load = ROM_03,	type = ro, define = no;
	ROM_04:		load = ROM_04,	type = ro, define = no;
	ROM_05:		load = ROM_05,	type = ro, define = no;
    ROM_06:		load = ROM_06,	type = ro, define = no;
    # Stubs that contain reset code to put ourselves into a known fixed-c000 state.
	STUB_00:   load = ROM_00, type = ro, start = $BFF0;
	STUB_01:   load = ROM_01, type = ro, start = $BFF0;
	STUB_02:   load = ROM_02, type = ro, start = $BFF0;
    STUB_03:   load = ROM_03, type = ro, start = $BFF0;
	STUB_04:   load = ROM_04, type = ro, start = $BFF0;
	STUB_05:   load = ROM_05, type = ro, start = $BFF0;
	STUB_06:   load = ROM_06, type = ro, start = $BFF0;

    CODE:     load = PRG,            type = ro,  define = yes;
    RODATA:   load = PRG,            type = ro,  define = yes;
    DATA:     load = PRG, run = RAM, type = rw,  define = yes;
    STUB_PRG: load = PRG_STUB,    type = ro, start = $FFF0;
    CHR_00:   load = CHR_00,            type = ro;
# More CHR_XX entries again, some other stuff
}

# More "feature" stuff ommitted here

SYMBOLS {

    __STACKSIZE__ = $0500;  	# 5 pages stack
	
    # WARNING: IF you change this next variable, you also need to update source/neslib_asm/crt0.asm - there is a similarly
    # named constant SYS_PRG_BANKS towards the top. Set it to the same value.
	NES_PRG_BANKS = 8; 			# number of 16K PRG banks, change to 2 for NROM256
	NES_CHR_BANKS = 16; 			# number of 8K CHR banks (If using 4k, divide by 2!)
    NES_MAPPER	  = 0; 			# mapper number
    NES_MIRRORING = 0;

}
```

Hopefully this doesn't look too scary. Anyway, we need to update threee things here. First, we need to update the
`MEMORY` section to have all 16 of our banks. We have 8 right now - `ROM_00` through `ROM_06`, then `PRG` is the 
last one. Copy the `ROM_06` line over 8 more times, and number them `ROM_07` through `ROM_0E`. The other parameters
on each line stay the same. 

Next, we need to update the `SEGMENTS` section - we need to do the same type of copying to point our new `ROM_07` 
through `ROM_0E` to the right places. (Note that there are two places to update in this section.) We also need
to do this a 3rd time to create the `STUB_07` through `STUB_0E` entries. These are used to put some code into the
rom that is required by the mapper. 

After this, in the `SYMBOLS` section at the bottom, we have to change `NES_PRG_BANKS` to `16` to reflect our higher
bank count.

Finally, there is a file named `tools/cc65_tools/game_constants.asm` - the value `SYS_PRG_BANKS` has to be changed to
`16` to match the value we set above. (The .asm file explains why this is)

This should do it; you have manually resized your rom. There is more detail on the syntax of the .cfg file here:
[ld65 documentation](https://www.cc65.org/doc/ca65-11.html)