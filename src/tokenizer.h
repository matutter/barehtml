
#ifndef HTML_TOKENIZER_H
#define HTML_TOKENIZER_H

#include "source_map.h"

// TODO: Repace this use of this enum with HTML_SOURCE_MAP_H
enum HTML_TOKEN_ID {
  TAG_START      = 10,
  TAG_START_CLOSE= 11,
  TAG_END        = 12,
  TAG_NAME       = 13,
  ATTR_EQ        = 20,
  ATTR_NAME      = 21,
  ATTR_VALUE     = 22,
  CONTENT        = 30,
  CONTENT_STYLE  = 31,
  CONTENT_SCRIPT = 33,
  COMMENT        = 40,
  HTML_END       = 50
};

#define TOKEN_ID_STR(x) \
   (TAG_START == x)       ? "TAG_START" \
  :(TAG_START_CLOSE == x) ? "TAG_START_CLOSE" \
  :(TAG_END == x)         ? "TAG_END" \
  :(ATTR_EQ == x)         ? "ATTR_EQ" \
  :(ATTR_NAME == x)       ? "ATTR_NAME" \
  :(ATTR_VALUE == x)      ? "ATTR_VALUE" \
  :(CONTENT == x)         ? "CONTENT" \
  :(CONTENT_STYLE == x)   ? "CONTENT_STYLE" \
  :(CONTENT_SCRIPT == x)  ? "CONTENT_SCRIPT" \
  :(COMMENT == x)         ? "COMMENT" \
  : "???"

int scan_html(char* html, int size, source_map_fn cb, void*);

#endif
