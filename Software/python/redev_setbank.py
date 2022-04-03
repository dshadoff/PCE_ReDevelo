# redev_setbank <page number> <bank number>
#
# Set an MMR bank value on the PC Engine
# Example: redev_setbank 4 0x86
#
import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)

if len(sys.argv) <= 2:
    print()
    print("Bad paramaters entered")
    print("Usage: redev_setbank <page number> <bank number>")
    exit()

error = 0
if sys.argv[1].isnumeric():
    page = int(sys.argv[1])
    if (page < 0) or (page > 7):
        error = 1
else:
    error = 1

bank = hexdecode(sys.argv[2])

if error == 1:
    print()
    print("Bad paramaters entered")
    print("Usage: redev_setbank <page number> <bank number>")
    exit()

ser = serial.Serial(comport, 19200, timeout=0.5)

ret, data = develo.setbank(ser, page, bank)
develo.chkret(ret, "setbank")
hexdump(data)

ser.close()

