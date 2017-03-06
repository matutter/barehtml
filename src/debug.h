#ifndef DEBUG_H
#define DEBUG_H

  #include <stdio.h>

  #define KRST  "\x1B[0m"
  #define KRED  "\x1B[31m"
  #define KGRN  "\x1B[32m"
  #define KYEL  "\x1B[33m"
  #define KBLU  "\x1B[34m"
  #define KMAG  "\x1B[35m"
  #define KCYN  "\x1B[36m"
  #define KWHT  "\x1B[37m"

  #define debug(fmt, args...) \
    printf(fmt KRST "\n", ##args);

  #define debug_info(fmt, args...) \
    debug(KCYN fmt, ##args)

  #define debug_danger(fmt, args...) \
    debug(KRED fmt, ##args)

  #define debug_warning(fmt, args...) \
    debug(KYEL fmt, ##args)

  #define debug_token(token) \
    debug( KMAG token": %s", yytext )

#endif