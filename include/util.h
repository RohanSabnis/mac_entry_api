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


typedef unsigned char u_char;
typedef int bool;
#define HASH_SIZE 	8
#define HASH_VAL 	11
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define MAC_ADDR_FMT_STR "%02x%02x.%02x%02x.%02x%02x"

#define MAC_ADDR_ARGS(mac_addr)                                  			\
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],    \
        mac_addr[5]

#define HASH_CALC(mac)	\
	((((unsigned int)mac[0] % HASH_VAL) ^ mac[4] ^ mac[5]) % HASH_SIZE)

void
increment_mac_address(uint8_t *mac, uint32_t inc);

char*
mac_type_to_string (mac_entry_type_t type);

/* Stack related functions */
void push(struct sNode** top_ref, topo_avl_node_t *t);
bool isEmpty(struct sNode *top);
topo_avl_node_t *pop(struct sNode** top_ref);

void mac_print(uchar *mac, char *mac_str);


#endif /* __UTIL_H__ */
