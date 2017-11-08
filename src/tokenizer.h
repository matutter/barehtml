
#ifndef HTML_TOKENIZER_H
#define HTML_TOKENIZER_H

#ifndef GC_HTML_INT
  #define GC_HTML_INT 0xcafedeed
#endif

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
  (TAG_START == x) ? "TAG_START" \
  :(TAG_START_CLOSE == x) ? "TAG_START_CLOSE" \
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
