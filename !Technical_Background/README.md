# PCE_ReDevelo - Technical Background

In 1998 or so, David Michel (author of the famous MagicEngine PC Engine emulator) released
MagicKit, an assembler package, which included some basic support for Develo.

At the time when it became availbale, I was able to get it to work enough to download the data
and programs from the CD-ROM disc, but I didn't do much more than that.

But more than just include the actual command-line utilities for using Develo on an IBM PC,
David included source code to be able to build and rebuild those utilities, and to build new
functionality using the basic building blocks.

Although - or perhaps because - it doesn't actually compile or work on modern PCs, this code
was used as the basis for the current work, and I'd like to credit David with making it work,
as well as to thank the original authors of the Develo system for making it possible.

There are only a few executables included in the MagicKit set:
- GETROM : This extracts the System Card ROM from a running system into a file on the PC
- EXECMX : This will send the program and data encoded as a '.MX' file (a standard encoding
  format used for burning ROMs/PROMs back in the day), and execute it
- PERUN  : This will send the program ('.MX' as above), and execute it, but then enter 'Slave' mode,
  so that the PC will run commands requested by the PC Engine

The original system, came with several additional utilities including an assembler, a MML compiler,
and "SLAVE", a program which would put the PC Engine into slave mode so that the PC Engine could
perform commands - such as list the current directory, create a file and send data into it, etc.

