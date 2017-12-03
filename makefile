.RECIPEPREFIX = |
.PHONY: grind-test test test-2 test-3

CC    = gcc
CINCL = -Isrc
CCFLAGS = -DDEBUG_ON -Wfatal-errors -g -ggdb

#bhtml.o: src/bhtml.c src/bhtml.yy.c src/bhtml.tab.c
#| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

src/tokenizer.o: src/tokenizer.c
| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

src/tag_id.o: src/tag_id.c
| ${CC} -c ${CCFLAGS} -fPIC ${CINCL} -o $@ $^

main.test: test/main.c src/bhtml.c src/tokenizer.o src/tag_id.o
| ${CC} ${CCFLAGS} ${CINCL} -o $@ $^

test: main.test
| ${bootstrap} ./$< test/*.html

test-1: main.test
| ${bootstrap} ./$< test/sample1.html

test-2: main.test
| ${bootstrap} ./$< test/sample2.html

test-3: main.test
| ${bootstrap} ./$< test/so.html

test-4: main.test
| ${bootstrap} ./$< test/*.html

grind-test: main.test
| valgrind ./$< test/sample1.html

grind-test-2: main.test
| valgrind ./$< test/*.html

clean:
| rm -f main.test src/tokenizer.o
