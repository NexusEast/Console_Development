What's this

This is a special neslib version with intergrated full blown FamiTracker music
driver combined with FamiTone2's sound effects player.

Since release of FamiTone I was getting complaints on how restricted it is, and
how missing features not allowing to create good music with it. It gets as far
as people now trying to expand the engine to support more FamiTracker features,
for some reason - rather pointless to me, as the very idea of FamiTone was to be
that restricted, the design was never intended to be expandable, and it would be
easier to just write another engine from scratch.

I also often heard that it is very difficult to add sound effects support into
FamiTracker player. Not to mention that it actually has its own rudimentary
sound effects system for a long time (seems that no one ever used it), that's
not true. This neslib version is the proof, or an example how to do it.

Hope it will enable creativity of those who had so much trouble with limited
feature set of FamiTone, and they will finally be able to get to making games.



Note that the music used in the example is from my game Lan Master. It has been
made within the first FamiTone limitations.



How to use

Create music in FamiTracker 0.4.6. Plain 2A03, no extra channels, no extra DPCM
banks. If you want to have a few songs in the game, create them in a single FTM
file, as sub songs. Once you're done, go File>Create NSF. Set type of file to
'BIN - Raw music data'. Save the data with its default names. There will be
either one or two files, depending from whether your music uses DPCM samples or
not. The names of the files will be music.bin and samples.bin. Put them to the
project folder instead of those that is already there. Be aware that
FamiTracker music data could be huge, so it may not fit the NROM. I'll leave
solving this issue to you.

If your song uses samples, you'll need to edit the CFG file, MEMORY section, to
allow them enough room. These lines:

    PRG: start = $8000, size = $7d00, file = %O ,fill = yes, define = yes;
	DMC: start = $fd00, size = $02fa, file = %O, fill = yes, define = yes;
	
Put DMC start as low as needed to fit your samples, but keep it aligned to 64
bytes. Then modify size of the PRG and DMC sections accordingly, so their
resulting size will be $7ffa.

To create sound effects, use the FamiTone2 package and nsf2data tool. Read the
readme there to get detailed instructions. End result should be sounds.s file,
put it into the project folder too.

Please note that all files were modified compared to the normal neslib, i.e.
crt0.s, neslib.s, the config file are all different. FamiTracker driver code
is also modified a bit, to use a buffer instead or direct APU writes. Use some
text compare tool if you need to learn the difference and move some code or
config settings from your normal neslib projects.

This neslib version does not have sample_play function, because FamiTracker
driver handles samples in a different way, and the function has been barely
used anyway.



mailto: shiru@mail.ru
