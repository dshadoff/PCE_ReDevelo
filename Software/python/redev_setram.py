import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


addr = 0x8000

print("Pushing Addr ",end='')
print('{0:0{1}X}'.format(addr,2))

f = open("bank0.bin", "rb")
data = f.read()
f.close()

ret = develo.setram(ser, addr, data)
develo.chkret(ret, "setram")
hexdump(data, addr)

ser.close()

