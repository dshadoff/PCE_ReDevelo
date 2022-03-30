import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)

ser = serial.Serial(comport, 19200, timeout=0.5)

ret, data = develo.getbank(ser)
develo.chkret(ret, "getbank")
hexdump(data)

ser.close()

