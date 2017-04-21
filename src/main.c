
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "libini.h"

char* readFile(char* path) {
  FILE* file = fopen (path, "rb");
  int length = 0;
  char* buffer = 0;
  
  if(file) {
    fseek (file, 0, SEEK_END);
    length = ftell(file);
    fseek (file, 0, SEEK_SET);
    buffer = malloc(length);
    
    if(buffer) {
      fread(buffer, 1, length, file);
    }
    
    fclose(file);
  }
  
  return buffer;
}

int main(int argc, char** argv) {

  if (argc >= 2) {
    
    char* path = argv[1];
    
    printf("Opening %s...\n", path);

    Object* objects;
    yyscan_t scanner;
    YY_BUFFER_STATE state;
    char* input = readFile(path);

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
      
  } else {
    debug_warning("No input provided")
  }
  
  return 0;
}