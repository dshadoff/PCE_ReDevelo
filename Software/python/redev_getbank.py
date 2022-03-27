import sys
import os
import serial


def dv_getbanks(port):
    port.write(b'B')
    port.flush()
    ret = port.read(1);
    if ret == b'\x00':
        banks = port.read(8)
    else:
        banks = b'\x00'
    return ret, banks


if 'DEVELO_PORT' not in os.environ:
    sys.exit("DEVELO_PORT not defined")

port = os.environ.get('DEVELO_PORT')
print("Using serial port: ", port)

ser = serial.Serial(port, 19200, timeout=0.1)

ret, data = dv_getbanks(ser)

if ret == b'\x00':
    for bank in data:
        print('{0:0{1}X}'.format(bank,2),end=' ')
    print("");
    print("OK",flush=True)
else:
    print('{0:0{1}X}'.format(ret,2))
    print("Not OK",flush=True)


ser.close()

