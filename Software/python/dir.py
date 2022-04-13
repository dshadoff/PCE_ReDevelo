# dir
#
# List all the files available for extract from the Develo disc
#
import sys
import os
import serial
import develo
from hextools import *

def showdir(entry):
    if entry[0] == 0x00:
        return
    if entry[11] == 0x00:
        return
    name = entry[0:8].decode('utf-8')
    if name == "        ":
        return
    ext  = entry[8:11].decode('utf-8') 
    fname  = name + "." + ext
    size   = (entry[22] * 16777216) + (entry[21] * 65536) + (entry[20] * 256) + entry[19]
    year   = ((entry[13] >> 1) + 80) % 100
    month  = ((entry[13] & 1)<<3)+(entry[12]>>5)
    day    = (entry[12] & 31)
    hour   = (entry[15] >> 3)
    minute = ((entry[15] & 7) <<3) + (entry[14]>>5)
    print(fname, end='  ')
    print('{0:8d}'.format(size), end=' ')
    print('{0:02d}-{1:02d}-{2:02d}'.format(year, month, day), end='   ')
    print('{0:02d}:{1:02d}'.format(hour, minute), end='   ')
    if entry[11] == 0x80:
        print('Trk     {0:2d}  Audio'.format(entry[16]),flush=True)
    else:
        sector = (entry[18]*65536) + (entry[17]*256) + entry[16]
        print('Rec {0:0{1}X}  Data'.format(sector,6) )

    
comport = develo.get_portnum()
print("Using serial port:", comport)
print()

ser = develo.open(comport)

endofdir = 0
for sector in range (2, 66):
    ret, data = develo.getsector(ser, sector)
    develo.chkret(ret, "getsector")

    for ind in range(0,64):
        entry = data[ind*32:(ind*32+32)]
        if entry[0] == 0x00:
            endofdir = 1
            break
        else:
            showdir(entry)

    if endofdir == 1:
        break
develo.close(ser)

