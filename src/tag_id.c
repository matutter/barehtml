#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "tag_id.h"

const tag_table_row tag_table[] = {
  { "script", TAG_SCRIPT },
  { "div",    TAG_DIV },
  { "style",  TAG_STYLE }
};

const int tag_table_size = sizeof(tag_table) / sizeof(tag_table[0]);

int html_lookup_tag_id(char* input) {

  if(!input) return TAG_NOT_A_TAG;
  if(strlen(input) <= 0) return TAG_NOT_A_TAG;

  for(int i = 0; i < tag_table_size; i++) {
    tag_table_row* row = (tag_table_row*)&tag_table[i];
    if(strncasecmp(row->tag, input, strlen(row->tag)) == 0) {
      return row->id;
    }
  }

  return TAG_USER_DEFINED;
};
