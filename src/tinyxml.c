
#include "tokenizer.h"
#include "tinyxml.h"
#include "debug.h"

int token_fn(token_t* tok) {
  int status = 0;

  switch(tok->id) {
    case TAG_START:
    case TAG_START_CLOSE:
    case TAG_END:
      dbg(KDIM KBLU "%s%s" KRST, tok->pad, tok->str);
      break;
    case TAG_NAME:
      dbg(KBOLD KCYN "%s%s" KRST, tok->pad, tok->str);
      break;
    case ATTR_EQ:
      dbg(KBOLD KWHT "%s%s" KRST, tok->pad, tok->str);
      break;
    case ATTR_NAME:
      dbg(KCYN "%s%s" KRST, tok->pad, tok->str);
      break;
    case ATTR_VALUE:
      dbg(KGRN "%s%s" KRST, tok->pad, tok->str);
      break;
    case CONTENT:
      dbg(KDIM KYEL "%s" KRST, tok->str);
      break;
    case CONTENT_STYLE:
      dbg(KDIM KMAG "%s" KRST, tok->str);
      break;
    case CONTENT_SCRIPT:
      dbg(KDIM KGRN "%s" KRST, tok->str);
      break;
    case COMMENT:
      dbg(KDIM KWHT "%s" KRST, tok->str);
      break;
    case HTML_END:
      dbg(KRST "\n");
      break;
    default:
      debug_danger("unknown token id %d", tok->id);
      break;
  }

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
