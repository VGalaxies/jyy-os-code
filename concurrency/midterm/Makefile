CC := gcc
CFLAGS := -pthread -lm -I../include
CFLAGS += -g
CFLAGS += -O2
# CFLAGS += -fsanitize=thread -fPIE -pie

.PHONY: clean data

FILE := $(shell pwd)
FILE := ${FILE}/midterm-v1.c

run: clean data ./foo.c $(FILE)
	$(CC) ./foo.c $(FILE) $(CFLAGS)
	./a.out < ./in

test: ./midterm-demo.c
	$(CC) ./midterm-demo.c $(CFLAGS)

clean:
	rm -f ./a.out ./in

# Makefile 中的变量以 $ 开头
# 为了避免和 shell 的变量冲突，shell 的变量以 $$ 开头
# for i in {1..100000}; do echo $((${i} * 2)) >> ./in; done
data:
	$(shell for i in {1..100000}; do echo $$(($${i} * 2)) >> ./in; done)

submit: $(FILE)
	curl http://jyywiki.cn/upload -F course=OS2022 -F module=Midterm -F token=$(TOKEN) -F file=@$(FILE)
