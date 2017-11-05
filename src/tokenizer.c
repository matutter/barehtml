#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <regex.h>

#include "debug.h"
#include "tokenizer.h"

#ifndef OK
  #define OK(x) (x == 0)
#endif

typedef struct scanner scanner;
struct scanner {
  char* pos;
  char* lpos;
  int size;
  char* html;
  char* end;
  int pad;
  token_cb cb;
  bool in_quote;
  char quote_char;
  bool bail_out;
};


char* get_scan_ptr(scanner*, size_t);
void finalize_scanner(scanner*);
char get_scan_char(scanner*, size_t);
int hard_scan_space(scanner*);
int soft_scan_space(scanner*);
int emit_token(scanner*, int);

/**
* Called after a token is emitted.
*/
void finalize_scanner(scanner* s) {
  s->lpos = s->pos;
  s->in_quote = false;
  s->quote_char = '\0';
  s->pad = 0;
}

char* get_scan_ptr(scanner* s, size_t offset) {
  return (s->pos + s->pad - offset);
}

char get_scan_char(scanner* s, size_t offset) {
  return *get_scan_ptr(s, offset);
}

int emit_token(scanner* s, int token_id) {

  token_t* tok = NULL;

  int tok_size = sizeof(token_t);

  int span_size = (s->pos - s->lpos);
  int pad_len = s->pad;
  int str_len = span_size - s->pad;

  #if 0
    debug_info(
      "tok: %d, "
      "pad: %d, "
      "str: %d, "
      "id: %d %s"
      , tok_size
      , pad_len
      , str_len
      , token_id
      , TOKEN_ID_STR(token_id)
      );
  #endif

  if(str_len <= 0 || pad_len < 0 || span_size <= 0) {
    debug_danger("invalid token calculation");
    s->bail_out = true;
    return -1;
  }

  char* token_ptr = (s->lpos + pad_len);

  void* ptr = calloc(1, tok_size + pad_len + str_len + 3);
  if(ptr) {

    tok = (token_t*)ptr;
    tok->pad = (char*)(ptr + tok_size + 1);
    tok->str = (char*)(ptr + tok_size + 1 + pad_len + 1);
    
    if(pad_len) {
      //debug_where("%ld-PAD", pad_size);
      strncpy(tok->pad, s->lpos, pad_len);
    } else {
      //debug_where("NULL-PAD");
      tok->pad[0] = '\0';
    }
    strncpy(tok->str, token_ptr, str_len);

    tok->id = token_id;
    tok->gc = GC_HTML_INT;
    tok->str_len = str_len;
    tok->pad_len = pad_len;

    s->cb(tok);

    free(tok);

  } else {
    debug_danger("scanner out of memory");
  }

  finalize_scanner(s);
  return 0;
} 

int scan_tag_end(scanner* s) {
  int status = -1;

  // if we get here we need to be in a tag
  soft_scan_space(s);
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

int scan_tag_start(scanner* s) {
  int status = -1;


  char* pos = get_scan_ptr(s, 0);
  while(s->pos < s->end) {

    switch(*s->pos) {
      case '<':
      case '!':
      case '?':
      case '/':
        s->pos++;
        break;
      default:
        if(pos < s->pos) {

          status = emit_token(s, TAG_START);
          return status;

        }
      break;
    }
  }

  return status;
}

/**
* `true` if `pos - offset` is after the address of `html`, else `false`.
*/
bool can_look_back(scanner* s, size_t offset) {
  size_t current_offset = (s->pos - s->html);
  return current_offset >= offset;
}

/**
* Sets the `in_quote` flag based on the current character being evaluated.
*/
void scan_escaped_text(scanner* s) {
  char c = get_scan_char(s, 0);
  if(s->in_quote && s->quote_char != '\0') {

    // last char is escape seq && we're not on the first character
    // we should never even get here on first char
    bool is_escaped = (can_look_back(s, 1) && (get_scan_char(s, -1) == '\\'));

    if(c == s->quote_char && !is_escaped) {
      s->in_quote = false;
      s->quote_char = '\0';
    }

  } else if(c == '\'' || c == '"') {
    s->in_quote = true;
    s->quote_char = c;
  }
}

/**
* `CONTENT` is everything between, before, and after tags.
*/
int scan_content(scanner* s) {
  int status = -1;

  char quote_char = '\0';
  while(s->pos < s->end) {
    switch(*s->pos) {
      case '"':
      case '\'':
        scan_escaped_text(s);
        break;
      case '<':

        if(!s->in_quote) {

          status = emit_token(s, CONTENT);
          return status;

        }

        break;
      default:
        break;
    }
    s->pos++;
  }

  return status;
}

/**
* WS can occur at semantically irrelevant places all over HTML, 
* it doesn't count as a token - but is necessary to reconstruct the original
* HTML without "reconstituting" it.
*/
int soft_scan_space(scanner* s) {

  char* pos = s->pos;
  int pad = 0;
  while((pos < s->end) && isspace(*pos)) {
    pos++;
    pad++;
  }

  s->pad = pad;

  return 0;
}

int hard_scan_space(scanner* s) {

  soft_scan_space(s);
  s->pos += s->pad;

  return 0;
}

int scan_attr_name(scanner* s) {
  #if 0
    debug_where("enter");
  #endif

  int status = soft_scan_space(s);
  if(OK(status)) {

    while(s->pos < s->end) {
      char c = get_scan_char(s, 0);
      if(isspace(c)) break;
      if(c == '=') break;
      s->pos++;
    }

    status = emit_token(s, ATTR_NAME);

  }

  #if 0
    debug_where("exit");
  #endif
  return status;
}

int scan_attr_value(scanner* s) {
  int status = -1;

  soft_scan_space(s);
  while(s->pos < s->end) {
    switch(get_scan_char(s, 0)) {
      case '\'':
      case '"':
        scan_escaped_text(s);
        break;
      case ' ':
      case '>':
        if(!s->in_quote) {
          return status = emit_token(s, ATTR_VALUE);
        }
        break;
      default:
        break;
    }

    s->pos++;
  }

  return status;
}

int scan_attribute(scanner* s) {

  int status = scan_attr_name(s);
  if(OK(status)) {
    soft_scan_space(s);
    if(get_scan_char(s, 0) == '=') {
      s->pos += 1;
      status = emit_token(s, ATTR_EQ);
      if(OK(status)) {
        status = scan_attr_value(s);
      }
    }
  }

  return status;
}

int scan_attributes(scanner * s) {
  int status = -1;
  regmatch_t match;
  regex_t re;

  status = scan_attr_name(s);
  if(OK(status)) {
    while(OK(status) && s->pos < s->end) {
      char* pos = s->pos;

      // should move `s->pos` forward, this is a mutable state
      status = scan_attribute(s);
      if(OK(status)) {
        // look for signs of end, or continue
        soft_scan_space(s);
        switch(get_scan_char(s, 0)) {
          case ' ':
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
  }

  return status;
}

int scan_document(scanner* s) {
  int status = 0;

  // the tricky part, documents start with a tag or content
  s->lpos = s->pos;

  while(OK(status) && (s->pos < s->end)) {
    char* pos = s->pos;

    if('<' == *s->pos) {

      status = scan_tag_start(s);
      if(OK(status)) {

        status = scan_attributes(s);
        if(OK(status)) {

          status = scan_tag_end(s);
          if(OK(status)) {

            s->bail_out=true;
          }
        }
      }
    } else {
      status = scan_content(s);
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

int scan_html(char* html, int size, token_cb cb) {
  int status = -1;

  scanner s = {
    .html = html,
    .size = size,
    .pos  = html,
    .lpos = html,
    .end  = (html + size),
    .in_quote = false,
    .cb   = cb
  };

  if(s.cb) {

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
