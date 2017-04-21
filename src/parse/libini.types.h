
#ifndef LIBINI_TYPES_H
#define LIBINI_TYPES_H

typedef struct INI_PROPERTY_T {
  char* key;
  char* value;
  struct INI_PROPERTY_T* next;
} Property;

typedef struct INI_OBJECT_T {
  char* key;
  struct INI_PROPERTY_T* properties;
  struct INI_OBJECT_T* next;
} Object;

struct INI_OBJECTS_MOD {
  struct INI_OBJECT_T* (*const new)(char*, struct INI_PROPERTY_T*);
  char* (*const stringify)(struct INI_OBJECT_T*);
  struct {
     struct INI_PROPERTY_T* (*const new)(char*, char*);
  } property;
};

const struct INI_OBJECTS_MOD Objects;

#define REVERSE_LIST(type, x) ({ \
  type* __tail = 0; \
  type* __head = x; \
  while (__head) { \
    type* __next = __head->next; \
    __head->next = __tail; \
    __tail = __head; \
    __head = __next; \
  } \
  x = __tail; \
})

#endif 