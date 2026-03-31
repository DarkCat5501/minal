# CC:=cc
# CC:=gcc
# CC:=clang
# CC:=x86_64-w64-mingw32-gcc
# GloablFlags:=-Wdollar-in-identifier-extension
SDL_INCLUDE=${shell pkg-config --cflags sdl3 sdl3-ttf}
SDL_LIBS=${shell pkg-config --libs sdl3 sdl3-ttf}
GloablFlags:=-fdollars-in-identifiers

-include tools/stdout.mk

.PHONY: app watch run

SRC_DIR:=./src
C_FILES:=$(call unique,${shell find ${SRC_DIR} -name "*.c"})
OBJ_FILES:=$(patsubst ${SRC_DIR}/%.c, build/%.o, ${C_FILES})
DEP_FILES:=$(OBJ_FILES:.o=.d)

ALL_DEPS:=${call unique,${FILES} ${foreach d,${DEP_FILES},${shell cat ${d} 2> /dev/null | sed -e "s/^[^:]*://" -e "s/\\\\//" | tr -s ' '} } }
-include $(DEP_FILES)


Info:=${ANSI-fg-yellow} ❱

tools/xflags: tools/xflags.c
	@echo -n "${Info} Tool [$@]: ${ANSI-fg-blue}"
	${CC} tools/xflags.c -o tools/xflags

build/%.o: ${SRC_DIR}/%.c 
	${call padd,"${Info} Building [$@]",80,=}
	@echo -n "${ANSI-dim}${ANSI-fg-blue}   ⏵ "
	@mkdir -p ${dir $@}
	${eval CFLAGS:=${shell ./tools/xflags -m CFLAGS $<}}
	${CC} -MMD -MP -MF ${patsubst build/%.o,build/%.d,$@} -c $< -o $@ ${CFLAGS} -I./ ${SDL_INCLUDES}
	@echo -n "${ANSI-reset}"

app: tools/xflags ${OBJ_FILES} 
	${call padd,"${Info} Linking: [$@]",80,=}
	@echo -n "${ANSI-dim}${ANSI-fg-blue}   ⏵ "
	${eval LFLAGS:=${call unique, ${foreach dir,${FILES}, ${shell ./tools/xflags -m LFLAGS ${dir} } } } }
	${CC} -o ./app ${OBJ_FILES} ${LFLAGS} ${GloablFlags} ${SDL_LIBS}
	@echo -n "${ANSI-reset}"

run: app
	${call padd,"${Info} Running [$<]",80,=}
	@echo "${ANSI-reset}"
	@./app

watch:
	@echo "Makefile ${ALL_DEPS}" | sed -e "s/^ +/ /" | tr -s ' ' '\n' | sort -u | entr -c -r make --no-print-directory run

