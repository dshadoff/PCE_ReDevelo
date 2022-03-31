import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


vaddr = 0x0000

print("Pushing Addr ",end='')
print('{0:0{1}X}'.format(vaddr,2))

f = open("vram_up.out", "rb")
data = f.read()
f.close()

ret = develo.setvram(ser, vaddr, data)
develo.chkret(ret, "setvram")
hexdump16(data, vaddr)

ser.close()

