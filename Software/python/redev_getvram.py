import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


vaddr = 0x0000

print("Fetching Addr ",end='')
print('{0:0{1}X}'.format(vaddr,2))

#ret, data2 = develo.getvram(ser, vaddr, 2048)
ret, data2 = develo.getvram(ser, vaddr, 2048)
develo.chkret(ret, "getvram")
hexdump16(data2, vaddr)


f = open("vram.out", "wb")
f.write(data2)
f.close()

ser.close()

