/*
 * --------------------------------------------
 * tables.h
 *
 * Contains all the various structures used for the mac-entry API utility
 */

#ifndef __TABLES_H__
#define __TABLES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "avltree.h"

typedef unsigned char uchar;
typedef avltree_t mac_tree;
typedef avltree_t mac_entry_avl_t;
typedef avltree_t topo_avl_node_t;
typedef uint32_t topo_id_t;

#define MAC_ADDR_BYTES 6
#define MAC_ADDR_STRING 19
#define TOPO_NAME_STRING_MAX_LEN 15
#define PORT_STRING_MAX_LEN 10
#define OK 0  /* No error */
#define ERR 1  /* er */
#define AGEOUT_VALUE 10

typedef enum {
    EOK = 0,
    EERR,
} err_types_t;

/**
 * MAC Entry type
 */
typedef enum {
    MAC_ENTRY_TYPE_INVALID = 0,
    MAC_ENTRY_TYPE_DYNAMIC,
    MAC_ENTRY_TYPE_STATIC,
} mac_entry_type_t;

/**
 * Topology entry
 */
typedef struct {
    /** Key: Topology id */
    topo_id_t topo_id;
    /* Data */
    char topo_name[TOPO_NAME_STRING_MAX_LEN];
    /** Per topo MAC entry table ptr */
    mac_tree *mac_tree;
} topo_entry_t;

/**
 * Type definition for the mac address.
 */
typedef struct {
    /** Linkage to topo entry in topo table */
    topo_entry_t *topo_entry_ptr;
    /** Key: MAC address data */
    uchar mac_address[MAC_ADDR_BYTES];
    /** Entry Type (Dynamic/Static) */
    mac_entry_type_t type;
    /** Port where MAC was learnt */
    char port[PORT_STRING_MAX_LEN];
    /** timestamp when mac was learnt */
    time_t learn_timestamp;
    size_t timer_id;
} mac_entry_t;

/* Structure of a stack node. Linked List implementation is used for
   stack. A stack node contains a pointer to tree node and a pointer to
   next stack node */
typedef struct sNode
{
    avltree_t *t;
    struct sNode *next;
} sNode_t;






#endif /* __TABLES_H__ */
