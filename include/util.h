/*
 * --------------------------------------------
 * util.h
 *
 * Contains the utilities used for the mac-entry API
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include "tables.h"

typedef int bool;
#define HASH_SIZE   8
#define HASH_VAL    11

#define HASH_CALC(mac)  \
    ((((unsigned int)mac[0] % HASH_VAL) ^ mac[4] ^ mac[5]) % HASH_SIZE)

char*
mac_type_to_string (mac_entry_type_t type);

/* Stack related functions */
void push(struct sNode** top_ref, topo_avl_node_t *t);
bool isEmpty(struct sNode *top);
topo_avl_node_t *pop(struct sNode** top_ref);

void mac_print(uchar *mac, char *mac_str);

#endif /* __UTIL_H__ */
