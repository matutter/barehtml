.RECIPEPREFIX = |
.PHONY: test

CC    = gcc
CINCL = -Isrc
CCFLAGS = -DDEBUG_ON -Wfatal-errors

#tinyxml.o: src/tinyxml.c src/tinyxml.yy.c src/tinyxml.tab.c
#| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

src/tinyxml.yy.c: src/lexer.flex
| flex --header-file=src/tinyxml.yy.h -o $@ -vd $^

src/tinyxml.tab.c: src/parser.bison
| bison --defines -o $@ $^

main.test: test/main.c src/tinyxml.c src/tinyxml.yy.c src/tinyxml.tab.c
| ${CC} ${CCFLAGS} ${CINCL} -o $@ $^

test: main.test
| ./main.test test/sample1.html

clean:
| rm -f main.test src/tinyxml.yy.h src/tinyxml.yy.c src/tinyxml.tab.c src/tinyxml.tab.h




