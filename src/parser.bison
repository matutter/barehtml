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

%token ENDF 0
%token VERSION ATTDEF ENDDEF EQ SLASH CLOSE END
%token <s> DOCTYPE ENCODING NAME VALUE DATA COMMENT START
%type <s> name_opt

%%

document
 : prolog element misc_seq_opt ENDF
 ;

prolog
 : version_opt encoding_opt doctype_opt
   misc_seq_opt
 ;

version_opt
 : VERSION      { debug_info("<?XML-VERSION 1.0?>"); }
 | /*empty*/
 ;

encoding_opt
 : ENCODING     { debug_info("<?XML-ENCODING %s?>", $1); }
 | /*empty*/
 ;

doctype_opt
 : DOCTYPE      { debug_info("%s", $1); }
 | /*empty*/
 ;


misc_seq_opt
 : misc_seq_opt misc
 | /*empty*/
 ;

misc
 : COMMENT                    { debug_info("%s", $1); }
 | attribute_decl
 ;

attribute_decl
 : ATTDEF NAME                { debug_info("<?XML-ATT %s", $2); }
   attribute_seq_opt ENDDEF {printf("?>");}
 ;  

element
 : START                      { debug_info("<%s", $1); }
   attribute_seq_opt
   empty_or_content
 ;

empty_or_content
 : SLASH CLOSE                { debug_info("/>"); }
 | CLOSE                      { debug_info(">"); }
   content END name_opt CLOSE { debug_info("</%s>", $5); }
 ;

content
 : content DATA     { debug_info("%s", $2); }
 | content misc
 | content element
 | /*empty*/
 ;
name_opt
 : NAME           { $$ = $1; }
 | /*empty*/      { $$ = strdup(""); }
 ;
attribute_seq_opt
 : attribute_seq_opt attribute
 | /*empty*/
 ;
attribute
 : NAME             { debug_info(" %s", $1); }
 | NAME EQ VALUE    { debug_info(" %s=%s", $1, $3); }
 ;

%%

int yyerror(doc_t* doc, yyscan_t scanner, const char *msg) {
  debug_danger("%s", msg);
}
