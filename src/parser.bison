%code requires {
  
  #include "tinyxml.types.h"
  #ifndef YY_TYPEDEF_YY_SCANNER_T
  #define YY_TYPEDEF_YY_SCANNER_T
    typedef void* yyscan_t;
  #endif

}

%{
 
  #include "debug.h"
  #include "tinyxml.tab.h"
  #include "tinyxml.yy.h"
 
  int yyerror(doc_t* doc, yyscan_t scanner, const char *msg);
 
%}

%define       api.pure full
%lex-param    { void * scanner }
%parse-param  { doc_t* doc }
%parse-param  { void * scanner }

%union {
  doc_t* doc;
  el_t* el;
  char* s;
}

%token EQ
%token TAG_OPEN TAG_END_OPEN TAG_CLOSE TAG_SP_OPEN TAG_SP_CLOSE
%token <s> NAME VALUE DATA EVALUE COMMENT

%%

document
  : elements end
  ;

end
  : { return 0; }
  ; 

elements
  : element
  | elements element
  ;

element
  : TAG_SP_OPEN tag_name attributes TAG_CLOSE
  | TAG_SP_OPEN attributes TAG_SP_CLOSE
  | TAG_OPEN attributes TAG_CLOSE content
  | TAG_END_OPEN NAME TAG_CLOSE
  | COMMENT
  ;

tag_name
  : NAME
  ;

attributes
  : attribute
  | attributes attribute
  ;

attribute
  : NAME
  | NAME EQ NAME
  | NAME EQ VALUE
  | NAME EQ EVALUE
  ;

content
  : content DATA
  | content element
  | /* empty */
  ;

%%

int yyerror(doc_t* doc, yyscan_t scanner, const char *msg) {
  debug_danger("%s", msg);
}
