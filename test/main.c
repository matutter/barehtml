
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "tinyxml.h"

char* read_file(char* path) {
  size_t size;
  size_t bytes_read;
  char* buffer = 0;
  
  FILE* fp = fopen(path, "rb");
  if(fp) {
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    buffer = malloc(size);
    
    if(buffer) {
      bytes_read = fread(buffer, 1, size, fp);
      if(size == bytes_read) {
        debug_where("finished reading %lu from %s", size, path);
      } else {
        debug_warning("failed to read %s", path);
      }
    }
    
    fclose(fp);
  }
  
  return buffer;
}

int main(int argc, char** argv) {

  if(argc < 2) {
    debug_warning("Missing argument, nothing to do...");
    return 1;
  }

  int status = 0;
  for(int i = 1; i < argc; i++) {
    char* path = argv[i];
    doc_t doc;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    status = yylex_init(&scanner);
    if(OK(status)) {
      char* xml = read_file(path);
      if(xml) {
        state = yy_scan_string(xml, scanner);
        
        status = yyparse(&doc, scanner);
        if(OK(status)) {
          debug_success("parse complete");
        }
        yy_delete_buffer(state, scanner);
      }
      yylex_destroy(scanner);
    }

    if(!OK(status)) {
      break;
    }
  }


  return status;
}