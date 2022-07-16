.PHONY: clean

box: ./dlbox.c
	gcc -g ./dlbox.c -o ./dlbox

dl: ./libc.S ./libhello.S ./main.S
	./dlbox gcc ./libc.S
	./dlbox gcc ./libhello.S
	./dlbox gcc ./main.S

clean:
	rm -f ./dlbox ./*.dl ./*.o

run: box dl
	./dlbox interp ./main.dl
