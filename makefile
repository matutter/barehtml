.RECIPEPREFIX = |
.PHONY: all clean run setup generate_sources

CC    = gcc
CINCL = -Isrc
CPPFLAGS = -DDEBUG_ON

SOURCES = $(wildcard src/parse/*.c)

all: setup generate_sources main.o

main.o: src/main.c
| ${CC} ${CPPFLAGS} ${CINCL} ${SOURCES} src/main.c -o main.o

setup:

generate_sources:
| make -C src/parse/ all

clean:
| make -C src/parse/ clean
| rm -f main.o

run: all
| ./main.o test/sample.ini

