
#include "tokenizer.h"
#include "tinyxml.h"
#include "debug.h"

char * get_highlight(int id) {

  switch(id) {
    case TAG_START:
    case TAG_START_CLOSE:
    case TAG_END:        return KDIM KBLU;
    case TAG_NAME:       return KBOLD KCYN;
    case ATTR_EQ:        return KBOLD KWHT;
    case ATTR_NAME:      return KCYN;
    case ATTR_VALUE:     return KGRN;
    case CONTENT:        return KDIM KYEL;
    case CONTENT_STYLE:  return KDIM KMAG;
    case CONTENT_SCRIPT: return KDIM KGRN;
    case COMMENT:        return KDIM KWHT;
    case HTML_END:       return KRST "\n";
    default:
      debug_danger("unknown token id %d", id);
      break;
  }

  return "";
}

int token_fn(token_t* tok) {
  int status = 0;

  char* highlight = get_highlight(tok->id);
  printf(
    "%s%.*s%.*s" KRST
    , highlight
    , tok->pad_len, tok->pad
    , tok->str_len, tok->str
  );

  return status;
}

int parse_html(char* html, int size) {
  int status = -1;

  debug_info("parsing document %d", size);

  status = scan_html(html, size, token_fn);
  if(OK(status)) {
    debug_success("scanner exited");



  } else {
    debug_warning("scanner encountered an error");
  }

  return status;
}
