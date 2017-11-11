
#include "tokenizer.h"
#include "source_map.h"
#include "tinyxml.h"
#include "debug.h"

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

int map_fn(source_map_t* map, void* arg) {
  int status = 0;

  printf(
    "%s%.*s%.*s" KRST
    , get_highlight(map->id)
    , map->ws_size, map->ws
    , map->text_size, map->text
  );

  return status;
}

int parse_html(char* html, int size) {
  int status = -1;

  debug_info("parsing document %d", size);

  status = scan_html(html, size, map_fn, NULL);
  if(OK(status)) {
    debug_success("scanner exited");



  } else {
    debug_warning("scanner encountered an error");
  }

  return status;
}
