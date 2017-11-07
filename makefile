.RECIPEPREFIX = |
.PHONY: grind-test test test-2

CC    = gcc
CINCL = -Isrc
CCFLAGS = -DDEBUG_ON -Wfatal-errors -g -ggdb

#tinyxml.o: src/tinyxml.c src/tinyxml.yy.c src/tinyxml.tab.c
#| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

src/tokenizer.o: src/tokenizer.c
| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

main.test: test/main.c src/tinyxml.c src/tokenizer.o
| ${CC} ${CCFLAGS} ${CINCL} -o $@ $^

test: main.test
| ./main.test test/sample1.html 

test-2: main.test
| ./main.test test/sample2.html 

grind-test: main.test
| valgrind ./main.test test/sample1.html 

clean:
| rm -f main.test src/tokenizer.o 




