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

