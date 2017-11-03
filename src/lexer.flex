%{
 
  #include <stdio.h>
  #include <ctype.h>
  #include "debug.h"
  #include "tinyxml.types.h"
  #include "tinyxml.tab.h"

  static int keep;

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

%}

%option warn nodefault
%option reentrant noyywrap never-interactive nounistd bison-bridge

nl        (\r\n|\r|\n)
ws        [ \t\r\n]+
open      {nl}?"<"
close     ">"{nl}?
namestart [A-Za-z\200-\377_]
namechar  [A-Za-z\200-\377_0-9.-]
esc       "&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
name      {namestart}{namechar}*
data      ([^<\n&]|\n[^<&]|\n{esc}|{esc})+
comment   {open}"!--"([^-]|"-"[^-])*"--"{close}
string    \"([^"&]|{esc})*\"|\'([^'&]|{esc})*\'
doctype   {open}"!DOCTYPE"{ws}{name}{ws}?{close}
version   {open}"?XML-VERSION 1.0?"{close}
encoding  {open}"?XML-ENCODING"{ws}{name}{ws}?"?"{close}
attdef    {open}"?XML-ATT"

%s CONTENT

%%

<INITIAL>{ws}       {/* skip */}
<INITIAL>{version}  { return VERSION; }
<INITIAL>{doctype}  { yylval->s = strdup(yytext); return DOCTYPE; }
<INITIAL>{encoding} { yylval->s = word(yytext + 14); return ENCODING; }
<INITIAL>"/"        { return SLASH; }
<INITIAL>"="        { return EQ; }
<INITIAL>{close}    { BEGIN(CONTENT); return CLOSE; }
<INITIAL>{name}     { yylval->s = strdup(yytext); return NAME; }
<INITIAL>{string}   { yylval->s = strdup(yytext); return VALUE; }
<INITIAL>"?"{close} { BEGIN(keep); return ENDDEF; }

{attdef}            {keep = YY_START; BEGIN(INITIAL); return ATTDEF;}
{open}{ws}?{name}   {BEGIN(INITIAL); yylval->s = word(yytext); return START;}
{open}{ws}?"/"      {BEGIN(INITIAL); return END;}
{comment}           {yylval->s = strdup(yytext); return COMMENT;}

<CONTENT>{data}     {yylval->s = strdup(yytext); return DATA;}

.                   { debug_danger("Unexpected character %c", *yytext); }
{nl}                {/* skip, must be an extra one at EOF */;}

%%
