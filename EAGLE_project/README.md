# PCE_ReDevelo - PC Boards

Here, you will find EAGLE project files for version of the board I have made/used.

The EAGLE project files are included, plus the gerbers and even the PCB assembly files
which might be useful for submission to JLCPCB (these are the BOM and assembly csv files).

### Revision A - Adafruit M0 Adalogger

This was the initial board I used, as I was familiar with it, and have made several designs
based on the board.  However, it has some features which I didn't end up using (and a cost
to go along with those features).

I ended up switching to the Seeeduino XIAO because this is basically a straightforward project.
So far, all code will work on both boards, but my target platform from now on will be the 'Rev B'
hardware, so this board may stop being supported at some point.

### Revision B - Seeeduino XIAO M0

As this project is straightforward, I decided to aim for a board with broad availability and
a low price (as well as a small footprint), so I chose the Seeeduino XIAO M0, which retails
for around US$5.50 .  This device is supported by the Arduino ecosystem, which is used by this
project at the current time.

In future, I may switch over to the Seeeduino XIAO RP2040 - which could sit on the same board,
but if I do that, I would port the Arduino code to the pico-sdk.

In this board revision, I have made the pads larger so that you can choose whether to mount the
XIAO M0 in a socket, or flush with the board (as the XIAO is castellated, and can be soldered
directly to the carrier board).  I also placed mounting holes in preparation for a 3D-printed
case, but have not yet designed one.  (One a case is designed, there may need to be another
board revision to accommodate the physical requirements of the case).

