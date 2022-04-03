# redev_getrom <filename>
#
# Read the CD System ROM (banks $00 through $1F) from the
# PC Engine via ReDevelo system, and save it to <filename>
#
import sys
import os
import serial
import develo
from hextools import *

comport = develo.get_portnum()
print("Using serial port: ", comport)
ser = serial.Serial(comport, 19200, timeout=1)

if len(sys.argv) <= 1:
    print()
    print("no filename entered")
    print("Usage: redev_getrom <filename>")
    exit()

f = open(sys.argv[1], "wb")

#
# save the intiial value of MMR #4 (address $8000-$9FFF)
#
ret, data = develo.getbank(ser)
develo.chkret(ret, "initial getbank")

savebank = data[4]

#
# Iterate through banks $00 through $1F, fetching
# data and save it to the file
#
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
#
# restore the original value of MMR #4 (address $8000-$9FFF)
#
ret = develo.setbank(ser, 4, savebank)
develo.chkret(ret, "setbank (revert)")

f.close()
ser.close()

