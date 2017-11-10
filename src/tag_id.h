#ifndef HTML_TAG_ID_H
#define HTML_TAG_ID_H

enum HTML_TAG_ID {
  TAG_NOT_A_TAG = -1,
  TAG_USER_DEFINED = 0,
  TAG_DIV = 1,
  TAG_SCRIPT = 2,
  TAG_STYLE = 3
};

#define HTML_TAG_ID_STR(x) \
   (TAG_NOT_A_TAG == x)    ? "TAG_NOT_A_TAG" \
  :(TAG_USER_DEFINED == x) ? "TAG_USER_DEFINED" \
  :(TAG_DIV == x)          ? "TAG_DIV" \
  :(TAG_SCRIPT == x)       ? "TAG_SCRIPT" \
  :(TAG_STYLE == x)        ? "TAG_STYLE" \
  : "???"

typedef struct tag_table_row tag_table_row;
struct tag_table_row {
  const char* tag;
  enum HTML_TAG_ID id;
};

int html_lookup_tag_id(char* input);

#endif