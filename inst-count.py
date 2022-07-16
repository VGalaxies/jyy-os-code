gdb.execute('set prompt off')
gdb.execute('set pagination off')
gdb.execute('starti')

n = 0
while True:
    n += 1
    try:
        gdb.execute('si')
    except:
        break

print(f'Instruction count: {n}')
gdb.execute('quit')
