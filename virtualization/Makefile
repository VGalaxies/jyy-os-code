FILE :=
CC := gcc
CFLAGS += -g
# CFLAGS += -O2

.PHONY: clean

run: $(FILE)
	$(CC) $(FILE) $(CFLAGS)

static: 
	gcc -static -fno-omit-frame-pointer -g ./dfs-fork.c -o dfs
	gcc -static -fno-omit-frame-pointer -g ./env.c -o env

clean:
	rm -f ./a.out ./a.txt ./dfs ./env
