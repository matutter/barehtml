%{
 
  #include <stdio.h>
  #include <ctype.h>
  #include "debug.h"
  #include "parse/libini.types.h"
  #include "parse/libini.tab.h"

  char* yy_strim_ws(char*);

%}


%option outfile="libini.yy.c" header-file="libini.yy.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd bison-bridge
 
HEADER  \[[^\]]+\]
STRING  ([^=#\n[]|"\\="|"\\#"|"\\["|"\\\n")+
COMMENT ^#.*$
EQ      "="
WS      [[:space:]]*

%%

{HEADER} {
  yytext[strlen(yytext)-1] = '\0';
  yylval->raw = yy_strim_ws(yytext + 1);
  return HEADER;
}

{EQ} { return EQ; }

{STRING} {
  yylval->raw = yy_strim_ws(yytext);
  return STRING;
}

{WS}      { }
{COMMENT} { }
.         { debug_warning("Unrecognized character '%s'", yytext); }

%%

char* yy_strim_ws(char* input) {
  char* dst = input;
  char* src = input;
  char* end = src + strlen(src) - 1;

  while(isspace(*src)) {
    ++src;
  }

  while(end > src && isspace(*end)) {
    *end-- = 0;
  }

  if(src != dst) {
    while ((*dst++ = *src++));
  }

  return input;
}
