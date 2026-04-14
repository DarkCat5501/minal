minal:
	gcc -ggdb -o build/minal minal.c ansi.c -lSDL3 -lSDL3_ttf -lm

run: minal
	./build/minal
