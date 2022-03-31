import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)


sector = 0

print("Fetching Sector ",end='')
print('{0:0{1}X}'.format(sector,6))

ret, data = develo.getsector(ser, sector)
develo.chkret(ret, "getsector")
hexdump(data, 0x0000)


f = open("sector.out", "wb")
f.write(data)
f.close()

ser.close()

