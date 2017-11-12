#ifndef DOM_TREE_H
#define DOM_TREE_H

#include "source_map.h"

typedef struct dom_node dom_node;
struct dom_node {

  dom_node* parent;
  dom_node** children;
  int child_count;

  source_map_t** source_map;

};


#endif