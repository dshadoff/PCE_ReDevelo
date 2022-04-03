# Hextools library for dealing with hexadecimal values
# and strings (used by Develo library and the Develo utility programs)
# 
def hexdecode(input):
    hexadecimal = 0
    num = 0
    if input[0] == "$":
        hexadecimal = 1
        hexnum = input[1:]
    elif input[0:2] == "0x" or input[0:2] == "0X":
        hexadecimal = 1
        hexnum = input[2:]

    if hexadecimal == 1:
        num = int(hexnum, 16)
    else:
        num = int(input)
    return num

def hexdump(memory, baseaddr = 'nul'):
    count = 0
    for x in memory:
        if count == 0:
            if baseaddr != 'nul':
                print('{0:0{1}X}'.format(baseaddr,4),end=': ')
        count = count + 1
        print('{0:0{1}X}'.format(x,2),end=' ')
        if count == 16:
            print()
            if baseaddr != 'nul':
                baseaddr = baseaddr + count
            count = 0
    if count != 0:
        print()

def hexdump16(memory, baseaddr = 'nul'):
    count = 0
    upperlower = 0
    for x in memory:
        if (count == 0) and (upperlower == 0):
            if baseaddr != 'nul':
                print('{0:0{1}X}'.format(baseaddr,4),end=': ')

        if upperlower == 0:
            temp = x
            upperlower = 1
        else:
            print('{0:0{1}X}'.format(x,2),end='')
            print('{0:0{1}X}'.format(temp,2),end=' ')
            count = count + 1
            upperlower = 0

        if count == 8:
            print()
            if baseaddr != 'nul':
                baseaddr = baseaddr + count
            count = 0
    if count != 0:
        print()

