import sys
import os
import serial
import develo
from hextools import *

if 'DEVELO_PORT' not in os.environ:
    sys.exit("DEVELO_PORT not defined")

comport = os.environ.get('DEVELO_PORT')
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)

if len(sys.argv) <= 1:
    print()
    print("no filename entered")
    print("Usage: redev_getrom <filename>")
    exit()

f = open(sys.argv[1], "wb")

ret, data = develo.getbank(ser)
savebank = data[4]
develo.chkret(ret, "initial getbank")

banknum = 0
while (banknum < 32):
    print("Fetching Bank ",end='')
    print('{0:0{1}X}'.format(banknum,2))

    ret = develo.setbank(ser, 4, banknum)
    develo.chkret(ret, "setbank (4, banknum)")

    ret, data2 = develo.getram(ser, 0x8000, 8192)
    develo.chkret(ret, "getram")
    f.write(data2)

    banknum = banknum + 1

ret = develo.setbank(ser, 4, savebank)
develo.chkret(ret, "setbank (revert)")

f.close()
ser.close()

