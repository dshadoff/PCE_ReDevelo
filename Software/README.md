# Host Side Software

The original Arduino code was originally envisioned as being self-contained, but this
was limiting, so a PC-based extension was created.

This uses a very rudimentary protocol over USB, using serial-based communication.
Very little effort was put into protocol synchronization or automatic recovery of
desynchronized sessions; this can be an exercise for later, if it becomes an issue.

The Host side is built using Python due to its ubiquity and general-purpose nature.

Initially, some key functions are being built to ensure that basic communication
works sufficiently well, and then core functions will be built into an importable
library.  More general-purpose programming may be able to take place from then.

There are two major bases of communication for Develo:
1. Host-initiated
   - These are requests by a user to view or change the internal state, such as:
     - get or set the bank (MMR) registers
     - get or set memory/VRAM/palette values
     - execute a progam
     - etc.

2. PC Engine-initiated
  - These functions are all initiated from within the original 'slave.exe' program, while
    the host system is placed into a state waiting for commands from the PC Engine system.
  - They are used primarily for initial download of the CD-ROM contents to the local
    disk, and include functions such as:
    - create file
    - send data
    - obtain directory listing
    - etc. 


## Host-initiated command Serial-link protocol

