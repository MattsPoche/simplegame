#!/bin/sh

cflags='-Wall -Wextra -Wswitch-enum -pedantic -std=c11 -ggdb'
lflags='-lSDL2 -lm -ldl -pthread'
glflags='-shared -fpic'

builddir='bin'
srcdir='src'

target='sg'
game='game.so'

game() {
	cc -o "$builddir/$game" "$srcdir/game.c" $cflags $glflags
}

sdl_linux() {
	cc -o "$builddir/$target" "$srcdir/sdl-linux_platform.c" $cflags $lflags
}

[ -d "$builddir" ] || mkdir -p "$builddir"

case $1 in
	'game') game ;;
	'sdl-linux') sdl_linux ;;
	'run') "./$builddir/$target" ;;
	*) sdl_linux && game ;;
esac
