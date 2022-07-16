import sys

limit = int(sys.argv[1])
count, n = 0, 100000
while True:
    for ch in sys.stdin.read(n):
        if ch == '(': count += 1
        if ch == ')': count -= 1
        assert 0 <= count <= limit
    print(f'{n} Ok.')
