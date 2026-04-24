minal: minal.c ansi.c
	gcc -ggdb -o build/minal $^ -lSDL3 -lSDL3_ttf -lm

ansi-test: tests/test-ansi.c ansi.c
	gcc -ggdb -o build/ansi-test tests/test-ansi.c ansi.c
	build/ansi-test

run: minal
	./build/minal
