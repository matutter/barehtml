#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <regex.h>

#include "debug.h"
#include "tag_id.h"
#include "tokenizer.h"
#include "source_map.h"

#ifndef OK
  #define OK(x) (x == 0)
#endif

#ifndef can_look_at
  /**
  * `true` if `pos - offset` is after the address of `html`, else `false`.
  */
  #define can_scan_at(s, loc) ( \
    (loc < 0) ? ((s->end + loc) > s->html) \
              : ((s->end - loc) > s->html) )
#endif

#ifndef count_padding
  #define scan_padding(s) ({ \
    s->pad = 0; \
    while(isspace(*s->pos)) s->pos++; \
    s->pad = (s->pos - s->lpos); \
    0; \
  })
#endif

typedef struct SCANNER_GUTS scanner;
struct SCANNER_GUTS {
  char* pos;
  char* lpos;
  int size;
  char* html;
  char* end;
  int pad;
  source_map_fn map_fn;
  void* arg;
  bool in_quote;
  char quote_char;
  bool bail_out;
  enum HTML_TOKEN_ID last_id;
  enum HTML_TAG_ID parent_tag_id;
  bool strict_content;
};


char* get_scan_ptr(scanner*, size_t);
void finalize_scanner(scanner*, int, char*);
char get_scan_char(scanner*, size_t);
int emit_token(scanner*, int);
int get_content_type(enum HTML_TAG_ID id);
int get_source_map(enum HTML_TOKEN_ID id);


static inline bool is_in_set(char c, char const* set) {
  while(*set) {
    if(*set == c) return true;
    set++;
  }
  return false;
}

static inline bool is_name_char(char c) {
  return (isalnum(c) || c == '_' || c == '-');
}

static inline bool is_quote_char(char c) {
  return (c == '\'' || c == '"');
}

static inline bool is_tag_end_char(char c) {
  return (c == '/' || c == '?' || c == '>');
}

static inline bool is_attr_value_char(char c) {
  // some special characters are allowed for evaluation as CSS properties
  // for example `width=100%` is valid
  const char allowed[] = "%!_-+():;";
  return (isalnum(c) || is_in_set(c, allowed));
}

/**
* Called after a token is emitted.
*/
void finalize_scanner(scanner* s, int id, char* str) {

  if(id == TAG_NAME) {
    if(s->last_id == TAG_START_CLOSE) {
      s->parent_tag_id = -1;
    } else {
      s->parent_tag_id =  html_lookup_tag_id(str);
    }

    if(s->parent_tag_id == TAG_SCRIPT || s->parent_tag_id == TAG_STYLE) {
      s->strict_content = true;
    }
  }

  s->last_id = id;
  s->pad = 0;
  s->lpos = s->pos;
  s->quote_char = '\0';
  s->in_quote = false;
}

char* get_scan_ptr(scanner* s, size_t offset) {
  return (s->pos + s->pad - offset);
}

char get_scan_char(scanner* s, size_t offset) {
  return *get_scan_ptr(s, offset);
}

int emit_token(scanner* s, int token_id) {

  int status = -1;
  int span_size = (s->pos - s->lpos);
  int pad_len = s->pad;
  int str_len = span_size - s->pad;
  char* token_ptr = (s->lpos + pad_len);

  if(pad_len < 0 || str_len < 0) {
    debug_warning("Invalid token offset calculation.");
    s->bail_out = true;
    return -1;
  }

  source_map_t sm = { 0 };
  sm.id = get_source_map(token_id);
  sm.text = token_ptr;
  sm.text_size = str_len;
  sm.ws = s->lpos;
  sm.ws_size = pad_len;
  sm.size = span_size;

  status = s->map_fn(&sm, s->arg);
  if(OK(status)) {
    finalize_scanner(s, token_id, token_ptr);
  } else {
    debug_warning("source_map_fn return status %d, leaving scanner...", status);
    s->bail_out = true;
    status = -1;
  }

  return status;
} 

int scan_tag_end(scanner* s) {
  int status = -1;

  // if we get here we need to be in a tag
  scan_padding(s);
  while(s->pos < s->end) {
    switch(*s->pos) {
      case '?': // some stupid XML thing... we all hate you XML
      case '/':
        s->pos++;
        break;
      case '>':
        s->pos++;
        status = emit_token(s, TAG_END);
        return status;
      default:
        if(isspace(*s->pos)) {
          s->pos++;
        } else {
          #if 0
            debug_warning("unexpected character");
          #endif
          status = -1;
        }
        break;
    }
  }

  return status;
}

int scan_comment(scanner* s) {
  int status = -1;

  // we may enter this scanner after a `<` has been consumed
  if(s->pos != s->lpos) {
    s->pos = s->lpos;
  }

  const char start_chars[] = "<!--";
  const char end_chars[] = "-->";

  char* comment_start = strstr(s->pos, start_chars);
  char* comment_end = strstr(s->pos, end_chars);

  if(comment_start && comment_end && comment_start == s->pos) {
    if(comment_start < comment_end) {
      s->pos = (comment_end + strlen(end_chars));
      s->pad = 0;
      status = emit_token(s, COMMENT);
    }
  } else {
    debug_warning("scanner not at beginning of comment");
    status = -1;
  }

  return status;
}

int scan_tag_start(scanner* s) {
  int status = -1;
  char* pos = s->pos;
  while(s->pos < s->end) {

    switch(*s->pos) {
      case '-':
        return -1;
      case '<':
      case '!':
      case '?':
      case '/':
        s->pos++;
        break;
      default:
        if(pos < s->pos) {

          int id = TAG_START;
          if(s->lpos == strstr(s->lpos, "</")) {
            id = TAG_START_CLOSE;
          }

          status = emit_token(s, id);
          return status;

        }
      break;
    }
  }

  return status;
}

/**
* Sets the `in_quote` flag based on the current character being evaluated.
*/
void scan_escaped_text(scanner* s, char* ptr) {
  char c;
  if(ptr) {
    c = *ptr;
  } else {
    c = get_scan_char(s, 0);
  }

  if(s->in_quote && s->quote_char != '\0') {

    // last char is escape seq && we're not on the first character
    // we should never even get here on first char
    bool is_escaped = (can_scan_at(s, -1) && (get_scan_char(s, -1) == '\\'));

    if(c == s->quote_char && !is_escaped) {
      s->in_quote = false;
      s->quote_char = '\0';
    }

  } else if(is_quote_char(c)) {
    s->in_quote = true;
    s->quote_char = c;
  }
}

int get_content_type(enum HTML_TAG_ID id) {
  switch(id) {
    case TAG_SCRIPT: return CONTENT_SCRIPT;
    case TAG_STYLE:  return CONTENT_STYLE;
    default:         return CONTENT;
  }
}

/**
* `CONTENT` is everything between, before, and after tags.
*/
int scan_content(scanner* s) {
  int status = -1;

  int id = get_content_type(s->parent_tag_id);
  while(s->pos < s->end) {
    char c = *s->pos;

    if(is_quote_char(c)) {
      scan_escaped_text(s, NULL);
      s->pos++;
      continue;
    }

    if(!s->in_quote) {
      if(c == '<') {

        // script must end with script tag
        if(s->strict_content) {
          char* pos = strstr(s->pos, "</");
          if(s->pos == pos) {
            status = emit_token(s, id);
            s->strict_content = false;
            return status;
          }
        } else {
          status = emit_token(s, id);
          return status;
        }
      }
    }

    s->pos++;
  }

  if(s->pos > s->lpos) {
    status = emit_token(s, id);
  }

  return status;
}

int scan_attr_name(scanner* s) {

  int id = ATTR_NAME;
  if(s->last_id == TAG_START || s->last_id == TAG_START_CLOSE) {
    id = TAG_NAME;
  }

  int status = -1;

  scan_padding(s);
  while(s->pos < s->end && is_name_char(*s->pos)) {
    s->pos++;
  }
  if(s->lpos < (s->pos)) {
    status = emit_token(s, id);
  } else {
    debug_warning("expected name chars, found nothing");
    status = -1;
  }

  return status;
}

int scan_attr_value(scanner* s) {
  int status = -1;

  scan_padding(s);
  bool value_ever = false;
  while(s->pos < s->end) {
    char c = *s->pos;

    if(is_quote_char(c)) {
      scan_escaped_text(s, s->pos);
      value_ever = true;
      s->pos++;
      continue;
    }

    // accept all in quote
    if(s->in_quote) {
      s->pos++;
      value_ever = true;
      continue;
    }

    while(s->pos < s->end && is_attr_value_char(c)) {

      #if 0
        debug_where("in attr_value loop (2)");
      #endif

      s->pos++;
      c = *s->pos;
      value_ever = true;
    }

    if(value_ever) {
      return status = emit_token(s, ATTR_VALUE);
    } else {
      debug_where("value never");
      // empty value is OK
      return 0;
    }
  }

  return status;
}

int scan_attribute(scanner* s) {

  #if 0
    debug_where("enter");
  #endif

  int status = scan_attr_name(s);
  if(OK(status)) {
    // we have a name, now scan for assignment to value
    scan_padding(s);
    while(s->pos < s->end) {

      #if 0
        debug_where("in attribute loop");
      #endif

      if(isspace(*s->pos)) {
        s->pos++;
        continue;
      }

      if(*s->pos == '=') {
        s->pos++;
        status = emit_token(s, ATTR_EQ);
        if(OK(status)) {
          status = scan_attr_value(s);
        }
        break;
      } else {
        status = 0;
        break;
      }
    }
  }

  #if 0
    debug_where("exit");
  #endif

  return status;
}

int scan_attributes(scanner * s) {
  int status = 0;

  while(OK(status) && s->pos < s->end) {
    char* pos = s->pos;

    #if 0
      debug_where("in attributes loop");
    #endif

    // should move `s->pos` forward, this is a mutable state
    status = scan_attribute(s);
    if(OK(status)) {
      // look for signs of end, or continue
      scan_padding(s);
      switch(get_scan_char(s, 0)) {
        case '/':
        case '>':
          return 0;
        default:
          break;
      }
    } else {
      break;
    }

    if(pos == s->pos) {
      break;
    }
  }

  return status;
}

int scan_document(scanner* s) {
  int status = 0;

  // the tricky part, documents start with a tag or content
  s->lpos = s->pos;

  while(OK(status) && (s->pos < s->end)) {
    char* pos = s->pos;

    #if 0
      debug_where("in document loop");
    #endif

    if('<' == *s->pos) {

      status = scan_tag_start(s);
      if(OK(status)) {

        status = scan_attributes(s);
        if(OK(status)) {

          status = scan_tag_end(s);
          if(OK(status)) {

          }
        }
      } else {
        status = scan_comment(s);
      }
    } else {
      status = scan_content(s);
    }

    if(s->pos == s->end) {
      return emit_token(s, HTML_END);
    }

    if(s->bail_out) {
      return -1;
    }

    if(pos == s->pos) {
      debug_warning("unexpected end of input");
      status = -1;
    }
  }

  return status;
}

int get_source_map(enum HTML_TOKEN_ID id) {
  switch(id) {
    case TAG_START:
    case TAG_START_CLOSE:
    case TAG_END:
      return HTML_ANCHOR;
    case TAG_NAME:       return HTML_TAG_NAME;
    case ATTR_NAME:      return HTML_ATTR_NAME;
    case ATTR_EQ:        return HTML_ATTR_EQ;
    case ATTR_VALUE:     return HTML_ATTR_VALUE;
    case CONTENT_SCRIPT: return HTML_CONTENT_SCRIPT;
    case CONTENT_STYLE:  return HTML_CONTENT_STYLE;
    case CONTENT:        return HTML_CONTENT_TEXT;
    case COMMENT:        return HTML_COMMENT;
    case HTML_END:       return 0;
    default:             return -1;
  }
}

int scan_html(char* html, int size, source_map_fn map_fn, void* arg) {
  int status = -1;

  scanner s = {
    .html = html,
    .size = size,
    .pos  = html,
    .lpos = html,
    .end  = (html + size),
    .in_quote = false,
    .map_fn   = map_fn,
    .arg = arg,
    .last_id = -1,
    .parent_tag_id = TAG_NOT_A_TAG,
    .strict_content = false
  };

  if(s.map_fn) {

    status = scan_document(&s);
    if(OK(status)) {
      debug_success("scan_document returned success");
    }

  } else {
    debug_warning("No callback provided");
    errno = EINVAL;
    status = -1;
  }

  return status;
}
