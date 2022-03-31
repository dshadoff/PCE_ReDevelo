import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


addr = 0x8000

print("Fetching Addr ",end='')
print('{0:0{1}X}'.format(addr,2))

ret, data = develo.getram(ser, addr, 8192)
develo.chkret(ret, "getram")
hexdump(data, addr)


f = open("ram.out", "wb")
f.write(data)
f.close()

ser.close()

