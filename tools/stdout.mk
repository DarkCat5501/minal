#Reset
ANSI-reset=\033[0m
#Foreground 
ANSI-fg-black=\033[30m
ANSI-fg-red=\033[31m
ANSI-fg-green=\033[32m
ANSI-fg-yellow=\033[33m
ANSI-fg-blue=\033[34m
ANSI-fg-purple=\033[35m
ANSI-fg-cyan=\033[36m
ANSI-fg-white=\033[37m
ANSI-fg-bright-black=\033[90m
ANSI-fg-bright-red=\033[91m
ANSI-fg-bright-green=\033[92m
ANSI-fg-bright-yellow=\033[93m
ANSI-fg-bright-blue=\033[94m
ANSI-fg-bright-purple=\033[95m
ANSI-fg-bright-cyan=\033[96m
ANSI-fg-bright-white=\033[97m
#Background
ANSI-bg-Black=\033[40m
ANSI-bg-Red=\033[41m
ANSI-bg-Green=\033[42m
ANSI-bg-Yellow=\033[43m
ANSI-bg-Blue=\033[44m
ANSI-bg-Purple=\033[45m
ANSI-bg-Cyan=\033[46m
ANSI-bg-White=\033[47m
ANSI-bg-bright-black=\033[100m
ANSI-bg-bright-red=\033[101m
ANSI-bg-bright-green=\033[102m
ANSI-bg-bright-yellow=\033[103m
ANSI-bg-bright-blue=\033[104m
ANSI-bg-bright-purple=\033[105m
ANSI-bg-bright-cyan=\033[106m
ANSI-bg-bright-white=\033[107m
#modes enable
ANSI-bold=\033[1m
ANSI-dim=\033[2m
ANSI-italic=\033[3m
ANSI-under=\033[4m
ANSI-blink=\033[5m
ANSI-rblink=\033[6m
ANSI-invert=\033[7m
ANSI-hide=\033[8m
ANSI-cross=\033[9m
#modes disable
ANSI-nunder=\033[21m
ANSI-normal=\033[22m
ANSI-nitalic=\033[23m
ANSI-nunder=\033[24m
ANSI-nblink=\033[25m
ANSI-ninvert=\033[27m
ANSI-reveal=\033[28m
ANSI-ncross=\033[29m

ANSI-font-0=\033[10m
ANSI-font-1=\033[11m
ANSI-font-2=\033[12m
ANSI-font-3=\033[13m
ANSI-font-4=\033[14m
ANSI-font-5=\033[15m
ANSI-font-6=\033[16m
ANSI-font-7=\033[17m
ANSI-font-8=\033[18m
ANSI-font-9=\033[19m

ANSI-over=\033[53m
ANSI-nover=\033[55m

ANSI-save=\033[s
ANSI-load=\033[u

ANSI-wrap=\033[?7h
ANSI-no-wrap=\033[?7l

define substr
${shell echo "$1" | cut -c${2}-${3}}
endef

define hex-to-rgb
${shell printf "%d %d %d" 0x${call substr,$1,1,2} 0x${call substr,$1,3,4} 0x${call substr,$1,5,6} }
endef

define color-red
${shell printf "%d" 0x${call substr,$1,1,2}}
endef

define color-green
${shell printf "%d" 0x${call substr,$1,3,4}}
endef

define color-blue
${shell printf "%d" 0x${call substr,$1,5,6}}
endef

define ANSI-fg
\033[38;2;${call get_r,$1};${call get_g,$1};${call get_b,$1}m
endef

define ANSI-fg
\033[48;2;${call get_r,$1};${call get_g,$1};${call get_b,$1}m
endef


define uniq
$(if $(filter $(1),$(2)),$(2),$(2) $(1))
endef

define unique
$(strip \
  $(eval _seen :=) \
  $(foreach w,$(1), \
    $(eval _seen := $(call uniq,$(w),$(_seen))) \
  ) \
  $(_seen))
endef

define padd
${eval LEN:=$(shell echo "$1 " | wc -c)}
${eval REMAIN:=${shell expr "$2" - ${LEN}}}
@echo -n $1 
@printf '%*s\n' ${REMAIN} '' | tr ' ' '${3}'
endef
