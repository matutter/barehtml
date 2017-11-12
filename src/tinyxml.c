#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tokenizer.h"
#include "source_map.h"
#include "dom_tree.h"
#include "tinyxml.h"
#include "debug.h"

typedef struct PARSER_GUTS parser;
struct PARSER_GUTS {

  union {
    char* ptr;
    char* html;
  };
  char* end_ptr;

  struct {
    source_map_t* list;
    // current index
    int idx;
    // number of slots
    int limit;
    // size of current allocation
    int size;
  } sm;

  source_map_t* sm_prev;
};

int get_memory_guess(char*, char*, int*, int*);

/**
* 
*/
char * get_highlight(int id) {

  switch(id) {
    case HTML_ANCHOR:         return KDIM KBLU;
    case HTML_TAG_NAME:       return KBOLD KCYN;
    case HTML_ATTR_NAME:      return KCYN;
    case HTML_ATTR_EQ:        return KBOLD KWHT;
    case HTML_ATTR_VALUE:     return KGRN;
    case HTML_CONTENT_TEXT:   return KDIM KYEL;
    case HTML_CONTENT_STYLE:  return KDIM KMAG;
    case HTML_CONTENT_SCRIPT: return KDIM KGRN;
    case HTML_COMMENT:        return KDIM KWHT;
    case 0:                   return KRST "\n"; // end
    default:
      debug_danger("unknown token id %d", id);
      break;
  }

  return "";
}

/**
* 
*/
int increase_parser_memory(parser* p, char* p1, char* p2) {

  int status = -1;

  int required_mem = 0;
  int item_count = 0;
  void* ptr = NULL;

  status = get_memory_guess(p1, p2, &item_count, &required_mem);
  if(OK(status)) {

    int total_mem = required_mem + p->sm.size;
    ptr = realloc(p->sm.list, total_mem);
    if(ptr) {

      debug_success("source map increased was: %d, increased: %d", p->sm.limit, item_count);

      p->sm.limit += item_count;
      p->sm.size = total_mem;
      p->sm.list = (source_map_t*)ptr;
      memset(&p->sm.list[p->sm.idx], 0, required_mem);
      status = 0;
    } else {
      debug_danger("failed to reallocate source map vector");
      status = -1;
    }
  }

  return status;
}

/**
* 
*/
int insert_source_map(parser* p, source_map_t* sm) {
  int status = -1;

  if(p->sm.idx >= p->sm.limit) {
    status = increase_parser_memory(p, sm->ptr, p->end_ptr);
    if(!OK(status)) {
      debug_danger("failed to increase parser memory");
      return -1;
    }
  }

  source_map_t* new_sm = &p->sm.list[p->sm.idx++];
  memcpy(new_sm, sm, sizeof(source_map_t));

  status = 0;

  return status;
}

/**
* 
*/
int map_fn(source_map_t* sm, void* arg) {
  int status = -1;

  parser* p = (parser*)arg;

  status = insert_source_map(p, sm);

  /*printf(
    "%s%.*s%.*s" KRST
    , get_highlight(sm->id)
    , sm->ws_size, sm->ws
    , sm->text_size, sm->text
  );*/

  return status;
}

int get_memory_guess(char* p1, char* p2, int* item_count, int* required_mem) {

  if(p1 >= p2) {
    debug_danger("p1: %p, p2: %p", p1, p2);
    errno = EINVAL;
    return -1;
  }

  // figure that a document will contain a lot of `<div> </div>` so we'll
  // guess that the ideal number of source_map_t we need is
  // remaining = (p2 - p1)
  // number = (remaining - (remaining % 12))
  const int avg_map = 26;
  const int size = sizeof(source_map_t);
  const int length = (p2 - p1);
  *item_count = (length - (length % avg_map)) / avg_map;

  if(*item_count < 10) *item_count = 10;

  *required_mem = *item_count * size;

  debug_info(
    "per-item: %d, remaining: %d, items: %d, memory: %d"
    , size
    , length
    , *item_count
    , *required_mem
  );

  return 0;
}

int parse_html(char* html, int html_size) {
  int status = -1;

  debug_info("parsing document %d", html_size);

  char* end = (html + html_size);
  int required_mem = 0;
  int item_count = 0;
  void* ptr = NULL;

  status = get_memory_guess(html, end, &item_count, &required_mem);
  if(!OK(status)) {
    debug_warning("Invalid inital memory calculation for source map.");
    return -1;
  }

  ptr = calloc(1, required_mem);
  parser p = {
    .html = html,
    .end_ptr = end,
    .sm = {
      .list = ptr,
      .idx = 0,
      .limit = item_count,
      .size = required_mem
    }
  };

  status = scan_html(html, html_size, map_fn, &p);
  if(OK(status)) {
    debug_success("scanner exited");

    debug_info("\n"
      " - map used: %d\n"
      " - map limit: %d\n"
      KBOLD " - map free: %d\n"
      KRST KDIM "----------------------\n" KRST KCYN
      " - mem total: %d\n"
      " - mem used: %d\n"
      KBOLD " - mem free: %d"
      , p.sm.idx
      , p.sm.limit
      , (p.sm.limit - p.sm.idx)
      , p.sm.size
      , (p.sm.idx * (int)sizeof(source_map_t))
      , (p.sm.size - (p.sm.idx * (int)sizeof(source_map_t)))
    );

  } else {
    debug_warning("scanner encountered an error");
  }

  return status;
}
