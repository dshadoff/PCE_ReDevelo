import sys
import os
import serial
import develo
from hextools import *


if 'DEVELO_PORT' not in os.environ:
    sys.exit("DEVELO_PORT not defined")

comport = os.environ.get('DEVELO_PORT')
print("Using serial port: ", comport)

ser = serial.Serial(comport, 19200, timeout=0.1)

ret, data = develo.getbank(ser)
develo.chkret(ret, "getbank")
hexdump(data)

ser.close()

