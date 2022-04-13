# getfile <filename>
#
# Download file from the Develo disc
#
import sys
import os
import time
import datetime
import serial
import develo
from hextools import *

def getfname(fnamein):
    dot = fnamein.find(".")
    length = len(fnamein)
    name = ""
    ext = ""
    if dot != -1:
        name = fnamein[0:dot]
        if dot != length:
            ext = fnamein[dot+1:]
    else:
        name = fnamein

    flatname = '{0:8s}{1:3s}'.format(name.upper(), ext.upper())
    return(flatname)


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

    
if len(sys.argv) <= 1:
    print()
    print("Bad paramaters entered")
    print("Usage: getfile <filename>")
    exit()

error = 0

fname = getfname(sys.argv[1])

if error == 1:
    print()
    print("Bad paramaters entered")
    print("Usage: getfile <filename>")
    exit()

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
            endofdir = 1    # End of list
            break
        else:
            if entry[0:11].decode('utf-8') == fname:
                endofdir = 2    # found
                showdir(entry)
                break

    if endofdir != 0:
        break

if endofdir == 2:   # found
    if entry[11] != 0x80:
        f = open(sys.argv[1], "wb")
        sector = (entry[18]*65536) + (entry[17]*256) + entry[16]
        size = (entry[22] * 16777216) + (entry[21] * 65536) + (entry[20] * 256) + entry[19]

        # loop to get 2KB at a time until done
        while size > 0:
            # print("sector = ", sector, "; size = ", size)
            ret, data = develo.getsector(ser, sector)
            develo.chkret(ret, "getsector")
            if size >= 2048:
                f.write(data)
            else:
                f.write(data[0:size])   # last (partial) sector
            size = size - 2048
            sector = sector + 1
        f.close()

        # update file date/time
        year   = ((entry[13] >> 1) + 80) % 100
        if year < 80:
            year = year + 2000
        else:
            year = year + 1900
        month  = ((entry[13] & 1)<<3)+(entry[12]>>5)
        day    = (entry[12] & 31)
        hour   = (entry[15] >> 3)
        minute = ((entry[15] & 7) <<3) + (entry[14]>>5)
        date = datetime.datetime(year=year, month=month, day=day, hour=hour, minute=minute, second=0)
        modTime = time.mktime(date.timetuple())
        os.utime(sys.argv[1], (modTime, modTime))
    else:
        print("Cannot read Audio Tracks at this time")
else:
    if endofdir == 1:   # not found
        print("File not found")

develo.close(ser)

