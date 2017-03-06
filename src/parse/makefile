.RECIPEPREFIX = |
.PHONY: all clean

all: libini.yy.c libini.tab.c
| echo "done"

libini.yy.c: libini_lexer.flex
| flex -vd libini_lexer.flex

libini.tab.c: libini_parser.bison
| bison -vd libini_parser.bison

clean:
| rm -f libini.yy.h libini.yy.c libini.tab.c libini.tab.h libini.output
