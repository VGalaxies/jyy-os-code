FILE :=
CC := gcc
CFLAGS := -pthread -lm -Iinclude
# CFLAGS += -g
CFLAGS += -O2
# CFLAGS += -fsanitize=thread -fPIE -pie

.PHONY: clean

run: $(FILE)
	$(CC) $(FILE) $(CFLAGS)

pc: $(FILE)
	$(CC) $(FILE) $(CFLAGS)
	./a.out 2 | python3 ./pc-check.py 2

mandelbrot: mandelbrot.c
	$(CC) mandelbrot.c $(CFLAGS)
	./a.out 32
	convert ./mandelbrot.ppm ./mandelbrot.jpg
	open ./mandelbrot.jpg

clean:
	rm -f ./a.out ./mandelbrot.ppm ./mandelbrot.jpg
