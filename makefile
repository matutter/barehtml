.RECIPEPREFIX = |
.PHONY: test

CC    = gcc
CINCL = -Isrc
CCFLAGS = -DDEBUG_ON -DYYDEBUG  -Wfatal-errors

#tinyxml.o: src/tinyxml.c src/tinyxml.yy.c src/tinyxml.tab.c
#| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

src/tinyxml.yy.c: src/lexer.flex
| flex --header-file=src/tinyxml.yy.h -o $@ -d $^

src/tinyxml.tab.c: src/parser.bison
| bison --defines -o $@ $^

main.test: test/main.c src/tinyxml.c src/tinyxml.yy.c src/tinyxml.tab.c
| ${CC} ${CCFLAGS} ${CINCL} -o $@ $^

test: main.test
| ./main.test test/google.html test/sample1.html test/sample2.html test/so.html

clean:
| rm -f main.test src/tinyxml.yy.h src/tinyxml.yy.c src/tinyxml.tab.c src/tinyxml.tab.h




