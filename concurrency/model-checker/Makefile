FILE :=

.PHONY: clean

run: $(FILE)
	python3 model-checker.py $(FILE) | python3 visualize.py > display.html

clean:
	rm -f display.html
