
#include <stdlib.h>
#include "debug.h"
#include "libini.h"

int main(void) {

  char input[] = ""
  "# line comment\n"
  "  [ abc//tty   ]  \n"
  " a =  b\n"
  "aa=bb  \n"
  //"  a123       =\"b456 asd\"\n"
  "  a123       =b456\\\ncontinued!"
  "  \n"
  "[obj2]\n"
  "pretty=true\n"
  ;

  Object* objects;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if(yylex_init(&scanner)) {
    return 1;
  }

  state = yy_scan_string(input, scanner);

  if(yyparse(&objects, scanner)) {
    return 1;
  }

  yy_delete_buffer(state, scanner);

  yylex_destroy(scanner);

  debug_info("%s", Objects.stringify(objects));

  return 0;
}