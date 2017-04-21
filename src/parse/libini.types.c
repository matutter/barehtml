#include <stdlib.h>
#include <stdio.h>
#include "parse/libini.types.h"
#include "debug.h"

#ifndef NULL
  #define NULL 0
#endif

static Object* newObject(char* key, Property* properties);
static char* stringifyObject(Object* obj);
static Property* newProperty(char* key, char* value);
static size_t calculateStringifyObjectSize(Object* obj);

const struct INI_OBJECTS_MOD Objects = {
  .new = newObject,
  .stringify = stringifyObject,
  .property = {
    .new = newProperty
  }
};

static Object* newObject(char* key, Property* properties) {
  Object* object = calloc(1, sizeof(Object));
  object->key = key;
  REVERSE_LIST(Property, properties);
  object->properties = properties;

  return object;
}

static size_t calculateStringifyObjectSize(Object* object) {
  
  Object* obj = object;
  int size = 1;

  while(obj) {
    size += snprintf(NULL, 0, "[%s]\n", obj->key);

    Property* prop = obj->properties;
    while(prop) {
      size += snprintf(NULL, 0, "%s = %s\n", prop->key, prop->value);
      prop = prop->next;
    }
    
    obj = obj->next;
  }

  return size;
}

static char* stringifyObject(Object* object) {
  
  Object* obj = object;
  int size = calculateStringifyObjectSize(obj);
  int written = 0;
  char* string = malloc(size);

  REVERSE_LIST(Object, obj);
  
  while(obj) {
    written += snprintf(string + written, size - written, "[%s]\n", obj->key);

    Property* prop = obj->properties;
    while(prop) {
      written += snprintf(string + written, size - written, "%s = %s\n", prop->key, prop->value);
      prop = prop->next;
    }
    
    obj = obj->next;
  }
  
  string[size-1] = '\0';
  
  return string;
}

static Property* newProperty(char* key, char* value) {
  Property* property = calloc(1, sizeof(Property));
  property->key = key;
  property->value = value;
  
  return property;  
}
