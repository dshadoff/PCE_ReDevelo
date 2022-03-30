import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


paladdr = 0x0000

print("Fetching Addr ",end='')
print('{0:0{1}X}'.format(paladdr,2))

ret, data2 = develo.getpal(ser, paladdr, 1024)
develo.chkret(ret, "getpal")
hexdump16(data2, paladdr)


f = open("paldata.out", "wb")
f.write(data2)
f.close()

ser.close()

