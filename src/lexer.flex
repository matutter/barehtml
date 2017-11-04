%{
 
  #include <stdio.h>
  #include <ctype.h>
  #include "debug.h"
  #include "tinyxml.types.h"
  #include "tinyxml.tab.h"

  static int in_tag;


  #if 0
    #define DBG_MODE() \
      dbg(KDIM KRED "(%d)" KRST, yyg->yy_start)
  #else
    #define DBG_MODE()
  #endif

  #ifndef dbg_tag
    #define dbg_tag() dbg(KDIM KBLU "%s" KRST, yytext)
  #endif

  static char* word(char *s) {
    char *buf;
    int i, k;
    for (k = 0; isspace(s[k]) || s[k] == '<'; k++) ;
    for (i = k; s[i] && ! isspace(s[i]); i++) ;
    buf = malloc((i - k + 1) * sizeof(char));
    strncpy(buf, &s[k], i - k);
    buf[i - k] = '\0';
    return buf;
  }

  //dbg("\n"); debug_info("Entered %s mode", name);
  #define MODE_SET(MODE, name) \
    BEGIN(MODE); 


%}

%option warn nodefault
%option reentrant noyywrap never-interactive nounistd bison-bridge


nl        (\r\n|\r|\n)
ws        [ \t\r\n]+

tag_open      "<"
tag_close     ">"
tag_end_open  "</"
tag_sp_open   "<"[!?]
tag_sp_close  "/>"

esc       "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
name      [a-zA-Z_][a-zA-Z0-9_\-]*
data      ([^<\n&]|&|\n[^<&]|\n{esc}|{esc}|"<=")+
comment   "<!--"([^-]|"-"[^-])*"-->"
string    (\"([^"])*\")|(\'([^'])*\')
e_value     [a-zA-Z0-9_\-%:]+

%s CONTENT

%%

<INITIAL>{ws} {
  dbg_token();
}

<INITIAL>"=" {
  dbg_token();
  return EQ;
}

{tag_open} {
  DBG_MODE();
  dbg_tag();
  MODE_SET(INITIAL, "INITIAL");
  DBG_MODE(); 
  return TAG_OPEN;
}

{tag_end_open}  {
  DBG_MODE(); 
  dbg_tag();
  MODE_SET(INITIAL, "INITIAL");
  DBG_MODE(); 
  return TAG_END_OPEN;
}

<INITIAL>{tag_close} {
  DBG_MODE(); 
  dbg_tag();
  MODE_SET(CONTENT, "CONTENT");
  DBG_MODE(); 
  return TAG_CLOSE;
}

<INITIAL>{tag_sp_open} {
  DBG_MODE(); 
  dbg_tag();
  DBG_MODE(); 
  return TAG_SP_OPEN;
}

<INITIAL>{tag_sp_close} {
  DBG_MODE(); 
  dbg_tag();
  MODE_SET(CONTENT, "CONTENT");
  DBG_MODE(); 
  return TAG_SP_CLOSE;
}

<INITIAL>{name} {
  dbg(KCYN "%s" KRST, yytext);
  yylval->s = strdup(yytext);
  return NAME;
}

<INITIAL>{string} {
  dbg(KGRN "%s" KRST, yytext);
  yylval->s = strdup(yytext); 
  return VALUE; 
}

<INITIAL>{e_value} {
  dbg(KMAG "%s" KRST, yytext);
  yylval->s = strdup(yytext); 
  return EVALUE;
}

<CONTENT>{data} {
  dbg(KYEL "%s" KRST, yytext);
  yylval->s = strdup(yytext); 
  return DATA;
}

{comment} {
  dbg(KDIM "%s" KRST, yytext);
  yylval->s = strdup(yytext); 
  return COMMENT;
}

{nl}                { dbg_token(); }
.                   { debug_danger("Unexpected character %c", *yytext); }

%%
