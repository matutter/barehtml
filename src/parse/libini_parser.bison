%output  "libini.tab.c"
%defines "libini.tab.h"

%code requires {
  
  #include "parse/libini.types.h"
  #ifndef YY_TYPEDEF_YY_SCANNER_T
  #define YY_TYPEDEF_YY_SCANNER_T
    typedef void* yyscan_t;
  #endif

}

%{
 
  #include "debug.h"
  #include "parse/libini.tab.h"
  #include "parse/libini.yy.h"
 
  int yyerror(Object** objects, yyscan_t scanner, const char *msg);
 
%}

%define       api.pure full
%lex-param    { yyscan_t scanner }
%parse-param  { Object** objects }
%parse-param  { yyscan_t scanner }

%union {
  char* raw;
  Object* obj;
  Property* prop;
}

%token EQ
%token<raw> HEADER STRING
%type<raw> key value header
%type<prop> property properties
%type<obj> stanza stanzas

%%

document
  : stanzas {
    *objects = $stanzas;
  }
  ;

stanzas
  : stanza {
    $$ = $stanza;
  }
  | stanzas[last] stanza[next] {
    $next->next = $last;
    $$ = $next;
  }
  ;

stanza
  : header properties {
    $$ = Objects.new($header, $properties);
  }
  ;

header
  : HEADER {
    $$ = $HEADER;
  }
  ;

properties
  : property {
    $$ = $property;
  }
  | properties[last] property[next] {
    $next->next = $last;
    $$ = $next;
  }
  ;

property
  : key EQ value {
    $$ = Objects.property.new($key, $value);
  }
  ;

key : STRING { $$ = strdup($STRING); };
value : STRING { $$ = strdup($STRING); };

%%

int yyerror(Object** objects, yyscan_t scanner, const char *msg) {
  debug_danger("%s", msg);
}
