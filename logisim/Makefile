.PHONY: clean

run: logisim.o
	./logisim | python3 seven-seg.py

display: logisim.o
	./logisim | python3 display.py

clean:
	rm -f logisim logisim.o
