# setpal <addr> <filename>
#
# Set data in the PC Engine's palette from a file
#   addr       = address
#   [filename] = filename to fetch data from
#
#
import sys
import os
import serial
import develo
from hextools import *

if (len(sys.argv) <= 2) or (len(sys.argv) > 3):
    print()
    print("bad parameters entered")
    print("Usage: setpal <addr> <filename>")
    exit()

paladdr  = hexdecode(sys.argv[1])

f = open(sys.argv[2], "rb")

comport = develo.get_portnum()
print("Using serial port: ", comport)

ser = develo.open(comport)

print("Pushing Palette Addr ",end='')
print('{0:0{1}X}'.format(paladdr,3))

data = f.read()
f.close()

ret = develo.setpal(ser, paladdr, data)
develo.chkret(ret, "setpal")

# hexdump16(data, paladdr)

ser.close()

