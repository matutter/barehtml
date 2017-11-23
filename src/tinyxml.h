#ifndef TINY_XML_H
#define TINY_XML_H

#include "tokenizer.h"

#ifndef OK
  #define OK(x) (x == 0)
#endif

int set_avg_map(int);
int parse_html(void* parse_ctx);
void* open_html_parser(char* html, int html_size);
void close_html_parser(void**);

#endif