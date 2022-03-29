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

print("Use filename: ", sys.argv[1])
print()


f = open(sys.argv[1], "wb")

ret, data = develo.getbank(ser)
savebank = data[4]
develo.chkret(ret, "initial getbank")
hexdump(data)

print("savebank=",end='')
print('{0:0{1}X}'.format(savebank,2))
print()

banknum = 0
while (banknum < 32):
    ret = develo.setbank(ser, 4, banknum)
    develo.chkret(ret, "setbank (4, banknum)")
    print("Setbank 4, banknum = OK",flush=True)

#develo.pr_from_port(ser)

    ret, data1 = develo.getbank(ser)
    develo.chkret(ret, "second getbank")
    hexdump(data1)
    print()

    baseaddr = 0x8000
    ret, data2 = develo.getram(ser, baseaddr, 8192)
    develo.chkret(ret, "getram")
    hexdump(data2, baseaddr)
    f.write(data2)
    print()

    banknum = banknum + 1



ret = develo.setbank(ser, 4, savebank)
develo.chkret(ret, "setbank (revert)")
print("Setbank 4, savebank = OK",flush=True)

ret, data = develo.getbank(ser)
develo.chkret(ret,"final getbank")
hexdump(data)
print()

ret = develo.err(ser)
develo.chkret(ret,"err")
print("force_ OK",flush=True)

f.close()
ser.close()

