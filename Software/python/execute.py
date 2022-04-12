# execute <address>
#
# Command PC Engine system to execute at the requested address
# Example: execute 0x4000
#
import sys
import os
import serial
import develo
from hextools import *

if len(sys.argv) <= 1:
    print()
    print("Bad paramaters entered")
    print("Usage: execute <address>")
    exit()

error = 0

bank = hexdecode(sys.argv[1])

if error == 1:
    print()
    print("Bad paramaters entered")
    print("Usage: setbank <page number> <bank number>")
    exit()

comport = develo.get_portnum()
print("Using serial port:", comport)
print()

ser = develo.open(comport)

addr =0x4000

ret = develo.exec(ser, addr)
develo.chkret(ret, "execute")

develo.close(ser)

