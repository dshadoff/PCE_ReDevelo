import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


paladdr = 0x0000

print("Pushing Plt Addr ",end='')
print('{0:0{1}X}'.format(paladdr,2))

f = open("paldata.out", "rb")
data = f.read()
f.close()

ret = develo.setpal(ser, paladdr, data)
develo.chkret(ret, "setpal")
hexdump16(data, paladdr)

ser.close()

