# getvram <addr> <len> [<filename>]
#
# Fetch data from the PC Engine's video memory
#   addr       = address
#   len        = length of data to fetch
#   [filename] = filename to store data into (optional)
#
# Data will be saved to <filename> if specified;
# If not specified, data will be printed on screen as hexdump
#
import sys
import os
import serial
import develo
from hextools import *

if (len(sys.argv) <= 2) or (len(sys.argv) > 4):
    print()
    print("bad parameters entered")
    print("Usage: getvram <addr> <len> [<filename>]")
    exit()

vaddr  = hexdecode(sys.argv[1])
length = hexdecode(sys.argv[2])

fileout = 0
if (len(sys.argv) == 4):
    f = open(sys.argv[3], "wb")
    fileout = 1

comport = develo.get_portnum()
print("Using serial port: ", comport)

ser = develo.open(comport)

if fileout == 0:
    print("Fetching Addr ",end='')
    print('{0:0{1}X}'.format(vaddr,2))

ret, data = develo.getvram(ser, vaddr, length)
develo.chkret(ret, "getvram")

if fileout == 0:
    hexdump16(data, vaddr)
else:
    f.write(data)
    f.close()

develo.close(ser)

