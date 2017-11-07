
#ifndef HTML_TOKENIZER_H
#define HTML_TOKENIZER_H

#ifndef GC_HTML_INT
  #define GC_HTML_INT 0xcafedeed
#endif

enum HTML_TOKEN_ID {
  TAG_START      = 1,
  TAG_END        = 2,
  ATTR_EQ        = 3,
  ATTR_NAME      = 4,
  ATTR_VALUE     = 5,
  CONTENT        = 6,
  CONTENT_STYLE  = 7,
  CONTENT_SCRIPT = 8,
  COMMENT        = 9,
  HTML_END       = 0
};

#define TOKEN_ID_STR(x) \
  (TAG_START == x) ? "TAG_START" \
  :(TAG_END == x) ? "TAG_END" \
  :(ATTR_EQ == x) ? "ATTR_EQ" \
  :(ATTR_NAME == x) ? "ATTR_NAME" \
  :(ATTR_VALUE == x) ? "ATTR_VALUE" \
  :(CONTENT == x) ? "CONTENT" \
  :(CONTENT_STYLE == x) ? "CONTENT_STYLE" \
  :(CONTENT_SCRIPT == x) ? "CONTENT_SCRIPT" \
  :(COMMENT == x) ? "COMMENT" \
  : "???"

typedef struct HTML_TOKEN token_t;
struct HTML_TOKEN {
  enum HTML_TOKEN_ID id;
  char* str;
  int str_len;
  char* pad;
  int pad_len;
  int gc;
};

typedef int (* token_cb)(token_t*);

int scan_html(char* html, int size, token_cb cb);

#endif
