all: bin aucont_start

bin:
	mkdir -p ../bin

aucont_start:
	gcc aucont_start.c -o ../bin/aucont_start -I./include


.PHONY: clean
clean:
	rm -f ../bin/*
	rm -f ../obj/*
