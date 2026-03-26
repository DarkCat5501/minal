minal:
	${eval SDL_INCLUDE=${shell pkg-config --cflags sdl3 sdl3-ttf}}
	${eval SDL_LIBS=${shell pkg-config --libs sdl3 sdl3-ttf}}
	gcc ${SDL_INCLUDE} -ggdb -o minal minal.c ${SDL_LIBS}

run: minal
	./minal
