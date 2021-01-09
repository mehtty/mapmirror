all:
	gcc -o mapmirror mapmirror.c
win:
	WINVER=1 x86_64-w64-mingw32-gcc -DWINVER -o mapmirror.exe mapmirror.c
