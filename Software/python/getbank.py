# getbank
#
# Fetch the current MMR bank settings from the PC Engine and display them
#
import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port:", comport)
print()

ser = develo.open(comport)

ret, data = develo.getbank(ser)
develo.chkret(ret, "getbank")
hexdump(data)

develo.close(ser)

