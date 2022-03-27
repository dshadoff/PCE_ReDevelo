# PCE_ReDevelo
A new set of hardware and support code based on Tokumashoten Intermedia's "Develo" magazines from 1996

In 1996, Tokumashoten Intermedia released several "MOOKs" (Magazine-style Books) related
to "Develo System". One for Assembler, one for BASIC, and two issues of a magazine which
followed.  The concept was that everyday users could learn how to program (or develop art
and/or music) for the PC Engine, and somehow they could eventually develop games of their own.

The "Develo System" consisted of three pieces:
1. Software on the PC Engine side (a CD-ROM included with the MOOK)
2. Software on the PC side (targetted at MSX and PC-98 series computers)
3. A small piece of hardware which was used to connect the two systems.  On the PC Engine
   side, it conected to the joypad port (while allowing pass-through for regular
   joypad use), and on the computer side, it connected to the printer port and RS-232
   port of the PC-98 (printer port and "Port2" of MSX).

Back in the day, I wasn't able to get this to work because the IBM PC systems available
in the West were not quite compatible with the PC-98. Also, I didn't know any Japanese,
and all of the documentation was in Japanese.

A version for IBM PC eventually came out, but the release packages were in an obscure
delta patch format sepcific to Japan, and the documentation was still all in Japanese.
I wasn't able to get it working to my satisfaction.

In 1998 or so, David Michel (author of the famous MagicEngine PC Engine emulator) released
an assembler package, which included some basic support for Develo, and I was able to
download the data on the disc, but I didn't do much more than that.

Years later, and computer nowadays don't normally have the same ports, and won't run the
same 16-bit progrmas from back then, so nothing from that time will work - unless you
manage to obtain old machines and put them to use.

This was the inspiration for the ReDevelo system I have created, which also consists of three pieces:
1. Software on the PC Engine side (use the original from the MOOK)
2. Software on the PC side (in Python, for flexibility)
3. A small piece of hardware to connect the two systems (and corresponding firmware).
   Plans are provided, including enough information for small-run production of the PC Boards.


