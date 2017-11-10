
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include "tinyxml.h"

int read_file(char* path, char** html, int* out_size) {
  int status = -1;
  size_t size;
  size_t bytes_read;
  
  FILE* fp = fopen(path, "rb");
  if(fp) {
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    *html = malloc(size+1);
    if(*html) {
      bytes_read = fread(*html, 1, size, fp);
      if(size == bytes_read) {
        debug_where("finished reading %lu from %s", size, path);
        *out_size = bytes_read;
        status = 0;
      } else {
        debug_warning("failed to read %s", path);
        free(*html);
        *html = NULL;
      }
    }
    
    fclose(fp);
  }
  
  return status;
}

int main(int argc, char** argv) {

  if(argc < 2) {
    debug_warning("Missing argument, nothing to do...");
    return 1;
  }

  int status = 0;
  for(int i = 1; i < argc; i++) {
    char* path = argv[i];

    char* html = NULL;
    int size = 0;
    status = read_file(path, &html, &size);
    if(OK(status) && html) {

      status = parse_html(html, size);
      if(OK(status)) {
        debug_success("parse success");
      }

      free(html);
    }

    if(!OK(status)) {
      break;
    }
  }


  return status;
}