# Develo CDROM layout

## Sectors

Sector 0 - Boot sector, a specific set of binary data to identify the disc as a PC Engine disc

Sector 1 - Startup sector; includes sectors to load, initial MMR settings and execute address

Sector 2 - 65 : This section is set up as a directory of files on the disc, to be accessed by
the PC host.  With each directory entry taking 32 bytes, this leaves 64 entries per sector, times
64 sectors, equals 4,096 possible files per disc.

### Directory Entries

Each directory entry is 32 bytes:
0-7: Filename
8-10: File extension
11: (if 0, ignore). If $80, audio track; otherwise data
12-13: Date bitfield.  Year = 7 MSb from 13. Month = LSb from 13, then 3 MSb from 12. Day = 5 LSb from 12
14-15: Time bitfield.  Hour = 5 MSb from 15. Minute = 3 LSb from 15, then 3 MSb from 14
If audio:
16: Track number
17-18: N/A
Else if data:
16-18: Sector number of data (18 is MSB, 16 is LSB of LBA value)
19-22: Size (22 is MSB, 19 is LSB)
23-32: Apparently unused

