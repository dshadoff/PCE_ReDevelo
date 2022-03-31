import os

def get_portnum():
    if 'DEVELO_PORT' not in os.environ:
        sys.exit("DEVELO_PORT not defined")
    return os.environ.get('DEVELO_PORT')

#
# Just print out any response information from the
# serial port
#
def pr_from_port(port):
    while 1:
        byte = port.read(1)
#       print(byte.decode('utf-8'),end='',flush=True)
        print(byte,end='',flush=True)

#
# Develo command to set a specific bank register
#   page = 0-7
#   bank = 0-255
#
def setbank(port, page, bank):
    port.write(b'A')
    port.write(page.to_bytes(1,'big'))
    port.write(bank.to_bytes(1,'big'))
    port.flush()
    ret = port.read(1)
    return ret

#
# Develo command to get all bank registers
#   returns a bytes object
#
def getbank(port):
    port.write(b'B')
    port.flush()
    ret = port.read(1);
    if ret == b'\x00':
        banks = port.read(8)
    else:
        banks = b'\x00'
    return ret, banks

#
# Develo command to get memory
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def getram_chunk(port, addr, numbytes):
    port.write(b'D')
    port.write(addr.to_bytes(2,'big'))
    port.write(numbytes.to_bytes(2,'big'))
    port.flush()
    ret = port.read(1);
    if ret == b'\x00':
        mem = port.read(numbytes)
    else:
        mem = b'\x00'
    return ret, mem

def getram(port, addr, numbytes):
    assert numbytes != 0, "getram: numbytes should be non-zero"
    count = 0
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        ret, mem = getram_chunk(port, addr, chunksize)
        if ret == b'\x00':
            if count == 0:
                allmem = mem
                count = count + 1
            else:
                allmem = allmem + mem
            addr = addr + chunksize
            numbytes = numbytes - chunksize
        else:
            allmem = b'\x00'
            break
    return ret, allmem

#
# Develo command to set memory
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def setram_chunk(port, addr, mem):
    port.write(b'E')
    port.write(addr.to_bytes(2,'big'))
    size = len(mem)
    port.write(size.to_bytes(2,'big'))
    port.flush()
    port.write(mem)
    port.flush()
    ret = port.read(1)
    return ret

def setram(port, addr, allmem):
    assert len(allmem) != 0, "setram: numbytes should be non-zero"
    start = 0
    end = len(allmem)
    numbytes = end - start
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        mem = allmem[start:(start+chunksize)]
        ret = setram_chunk(port, addr, mem)
        if ret == b'\x00':
            start = start + chunksize
            addr = addr + chunksize
            numbytes = end - start
        else:
            break
    return ret


#
# Develo command to get VRAM
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def getvram_chunk(port, addr, numbytes):
    port.write(b'F')
    port.write(addr.to_bytes(2,'big'))
    port.write(numbytes.to_bytes(2,'big'))
    port.flush()
    ret = port.read(1);
    if ret == b'\x00':
        mem = port.read(numbytes)
    else:
        mem = b'\x00'
    return ret, mem

def getvram(port, addr, numbytes):
    assert numbytes != 0, "getvram: numbytes should be non-zero"
    count = 0
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        ret, mem = getvram_chunk(port, addr, chunksize)
        if ret == b'\x00':
            if count == 0:
                allmem = mem
                count = count + 1
            else:
                allmem = allmem + mem
            addr = addr + int(chunksize / 2)
            numbytes = numbytes - chunksize
        else:
            allmem = b'\x00'
            break
    return ret, allmem

#
# Develo command to set memory
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def setvram_chunk(port, addr, mem):
    port.write(b'G')
    port.write(addr.to_bytes(2,'big'))
    size = len(mem)
    port.write(size.to_bytes(2,'big'))
    port.flush()
    port.write(mem)
    port.flush()
    ret = port.read(1)
    return ret

def setvram(port, addr, allmem):
    assert len(allmem) != 0, "setvram: numbytes should be non-zero"
    start = 0
    end = len(allmem)
    numbytes = end - start
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        mem = allmem[start:(start+chunksize)]
        ret = setvram_chunk(port, addr, mem)
        if ret == b'\x00':
            start = start + chunksize
            addr = addr + int(chunksize/2)
            numbytes = end - start
        else:
            break
    return ret

#
# Develo command to get palette data
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def getpal_chunk(port, addr, numbytes):
    port.write(b'H')
    port.write(addr.to_bytes(2,'big'))
    port.write(numbytes.to_bytes(2,'big'))
    port.flush()
    ret = port.read(1);
    if ret == b'\x00':
        mem = port.read(numbytes)
    else:
        mem = b'\x00'
    return ret, mem

def getpal(port, addr, numbytes):
    assert numbytes != 0, "getpal: numbytes should be non-zero"
    count = 0
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        ret, mem = getpal_chunk(port, addr, chunksize)
        if ret == b'\x00':
            if count == 0:
                allmem = mem
                count = count + 1
            else:
                allmem = allmem + mem
            addr = addr + int(chunksize / 2)
            numbytes = numbytes - chunksize
        else:
            allmem = b'\x00'
            break
    return ret, allmem

#
# Develo command to set memory
#   returns a bytes object
#
#  NOTE: Each remote fetch limited to 512 bytes
#
def setpal_chunk(port, addr, mem):
    port.write(b'I')
    port.write(addr.to_bytes(2,'big'))
    size = len(mem)
    port.write(size.to_bytes(2,'big'))
    port.flush()
    port.write(mem)
    port.flush()
    ret = port.read(1)
    return ret

def setpal(port, addr, allmem):
    assert len(allmem) != 0, "setpal: numbytes should be non-zero"
    start = 0
    end = len(allmem)
    numbytes = end - start
    while numbytes > 0:
        chunksize = min(numbytes, 512)
        mem = allmem[start:(start+chunksize)]
        ret = setpal_chunk(port, addr, mem)
        if ret == b'\x00':
            start = start + chunksize
            addr = addr + int(chunksize/2)
            numbytes = end - start
        else:
            break
    return ret

#
# Develo test command to return an error code
# (not part of original system)
#    returns a return code
#
def err(port):
    port.write(b'Z')
    port.flush()
    ret = port.read(1);
    return ret

#
# Evaluate return code from develo command
# and print corresponding error message
#
def chkret(ret, text):
    if ret != b'\x00':
        print("Develo error",end=' ')
        print('{0:0{1}X}'.format(ret[0],2),end=' ')
        print("in",end=' ')
        print(text,end='. Terminating.')
        print()
        exit(1)

