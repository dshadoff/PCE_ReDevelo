# setvram <addr> <filename>
#
# Set data in the PC Engine's video memory from a file
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
    print("Usage: setvram <addr> <filename>")
    exit()

vaddr  = hexdecode(sys.argv[1])

f = open(sys.argv[2], "rb")

comport = develo.get_portnum()
print("Using serial port: ", comport)

ser = develo.open(comport)

print("Pushing Addr ",end='')
print('{0:0{1}X}'.format(vaddr,4))

data = f.read()
f.close()

ret = develo.setvram(ser, vaddr, data)
develo.chkret(ret, "setvram")

# hexdump16(data, vaddr)

ser.close()

