#ifndef HTML_TAG_ID_H
#define HTML_TAG_ID_H

enum HTML_TAG_ID {
  TAG_NOT_A_TAG,
  TAG_DIV,
  TAG_SCRIPT,
  TAG_STYLE,
  TAG_USER_DEFINED
};

typedef struct tag_table_row tag_table_row;
struct tag_table_row {
  const char* tag;
  enum HTML_TAG_ID id;
};

int html_lookup_tag_id(char* input);

#endif