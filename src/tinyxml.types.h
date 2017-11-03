#ifndef TINY_XML_TYPES_H
#define TINY_XML_TYPES_H

#ifndef OK
  #define OK(x) (x==0)
#endif

typedef struct xml_element el_t;
struct xml_element {
  el_t* parent;
  el_t** children;
  char* tag;
  char* text;
};

typedef struct xml_document doc_t;
struct xml_document {
  el_t* root;
};

#endif