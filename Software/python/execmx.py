# execmx <filename>
#
# Set data in the PC Engine's main memory from a ".MX" file, and execute it
#   [filename] = filename to fetch data from
#
# Current limitations:
#  - Only supports 'S2' records (as created by Develo)
#  - Does not support discontiguous output
#  - May not properly handle files larger than 8KB
#
import sys
import os
import serial
import develo
from hextools import *

if (len(sys.argv) <= 1) or (len(sys.argv) > 2):
    print()
    print("bad parameters entered")
    print("Usage: execmx <filename>")
    exit()

f = open(sys.argv[1], "rb")
lines = f.readlines()
f.close

addrlow  = 0xffff
addrhigh = 0
nextaddr = 0
data     = b''      # empty bytes structure which we will build as we read the MX file
errcode  = 0

for line in lines:
    if line[0:1].decode('utf-8') != "S":
        print('{0} record found - invalid record type'.format(line[0:2].decode('utf-8')))
        errcode = 1
        break
    rectype = line[1:2].decode('utf-8')
    if rectype == "8":      # normal end of file - we ignore entry address though
        break
    if rectype != "2":      # Develo only used S2 (24-bit address) and S8 (execution addr) records
        print('S{0} record found - invalid record type'.format(rectype))
        errcode = 1
        break
    length = int(line[2:4].decode('utf-8'), 16) - 4
    addr   = int(line[6:10].decode('utf-8'), 16)
    if nextaddr != 0 and addr != nextaddr:
        print("not contiguous - break")
        errcode = 1
        break
#    print("addr = ", addr, "length = ", length)
    nextaddr = addr + length
    addrlow  = min(addrlow, addr)
    addrhigh = max(addrhigh, nextaddr)
    # decode bytes and build data array
    data = data + bytes.fromhex(line[10:(10+(length*2))].decode('utf-8'))

if errcode != 0:
    print("Error. Terminating")
    exit()

addr = addrlow

comport = develo.get_portnum()
print("Using serial port: ", comport)

ser = develo.open(comport)

ret, banks = develo.getbank(ser)
develo.chkret(ret, "getbank")

#print("Bank 2 = ",end='')
#print('{0:0{1}X}'.format(banks[2],1))

ret = develo.setbank(ser, 3, banks[2]+4)
develo.chkret(ret, "setbank 3")

ret = develo.setbank(ser, 4, banks[2]+5)
develo.chkret(ret, "setbank 4")

ret = develo.setbank(ser, 5, banks[2]+6)
develo.chkret(ret, "setbank 5")

ret = develo.setbank(ser, 6, banks[2]+7)
develo.chkret(ret, "setbank 6")

print("Sending data: Addr ",end='')
print('{0:0{1}X}'.format(addr,2),end='')
print(" - ",end='')
print('{0:0{1}X}'.format(addrhigh,2))

ret = develo.setram(ser, addr, data)
develo.chkret(ret, "setram")

print("Executing Addr ",end='')
print('{0:0{1}X}'.format(addr,2))

ret = develo.exec(ser, addr)
develo.chkret(ret, "exec")
#hexdump(data, addr)

ser.close()

