#!/bin/sh

CC='gcc'

cflags='-Wall -Wextra -Wswitch-enum -pedantic -std=c11 -ggdb'
lflags='-lSDL2 -lm -ldl -pthread'
glflags='-shared -fpic'
releaseflags='-std=c11 -O2'

builddir='bin'
srcdir='src'

target='sg'
game='game.so'

[ -d "$builddir" ] || mkdir -p "$builddir"

game() {
	$CC -o "$builddir/$game" "$srcdir/game.c" $cflags $glflags
}

sdl_linux() {
	$CC -o "$builddir/$target" "$srcdir/sdl-linux_platform.c" $cflags $lflags
}

release() {
	$CC -o "$builddir/$target" "$srcdir/sdl-linux_platform.c" $releaseflags $lflags
	$CC -o "$builddir/$game" "$srcdir/game.c" $releaseflags $glflags
}

case $1 in
	'game') game ;;
	'sdl-linux') sdl_linux ;;
	'release') release ;;
	'run') "./$builddir/$target" ;;
	*) sdl_linux && game ;;
esac
