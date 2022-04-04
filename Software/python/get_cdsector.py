# get_cdsector <sector> [<filename>]
#
# Fetch the designated sector from the CD
#   sector                = offset from start of data track
#   [filename] (optional) = filename to store data into
#
# Data will be saved to <filename> if specified;
# If not specified, data will be printed on screen as hexdump
#
import sys
import os
import serial
import develo
from hextools import *

if (len(sys.argv) <= 1) or (len(sys.argv) > 3):
    print()
    print("bad parameters entered")
    print("Usage: get_sector <sector> [<filename>]")
    exit()

sector = hexdecode(sys.argv[1])

fileout = 0
if (len(sys.argv) == 3):
    f = open(sys.argv[2], "wb")
    fileout = 1

comport = develo.get_portnum()
print("Using serial port: ", comport)
print()

ser = develo.open(comport)

if fileout == 0:
    print("Fetching Sector ",end='')
    print('{0:0{1}X}'.format(sector,6))

ret, data = develo.getsector(ser, sector)
develo.chkret(ret, "getsector")

if fileout == 0:
    hexdump(data, 0x0000)
else:
    f.write(data)
    f.close()

develo.close(ser)

