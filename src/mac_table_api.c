/*
 ============================================================================
 Name        : mac-table-api.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : API Manager for MAC table
 ============================================================================
 */

#include "util.h"
#include "timer.h"

static avltree_t *topo_root = 0x0;

static void
mac_expiration_timer_handler (size_t timer_id, void *user_data);

void
delete_mac_entry_for_given_topo (uchar  *mac,
								 topo_id_t topo_id);

void add_mac_entry_to_timer (mac_entry_avl_t *mac_node)
{
	size_t timer;

	timer = start_timer(AGEOUT_VALUE, mac_expiration_timer_handler, TIMER_SINGLE_SHOT, mac_node);
	((mac_entry_t *)mac_node->data)->timer_id = timer;
	printf("Mac entry added to expiry timer(%d)\n", timer);
}

void remove_mac_entry_from_timer (mac_entry_avl_t *mac_node)
{
	size_t timer;
	char mac_str[MAC_ADDR_STRING];


	if (!mac_node || !mac_node->data) return;

	timer = ((mac_entry_t *)mac_node->data)->timer_id;

	memset(mac_str, 0, MAC_ADDR_STRING);
	mac_print(((mac_entry_t *)avltree_get_data(mac_node))->mac_address,
			mac_str);

	printf("Removing MAC from timer(%d) for TOPO(%u) %s"
			"\n", timer, ((mac_entry_t *)avltree_get_data(mac_node))->topo_entry_ptr->topo_id,
			mac_str);
	stop_timer(timer);
	((mac_entry_t *)mac_node->data)->timer_id = 0x0;
}

void
mac_expiration_timer_handler (size_t timer_id, void *user_data)
{
	time_t now_ts;
	mac_entry_t *mac_entry = NULL;

	time(&now_ts);
	mac_entry_avl_t *mac_node = (mac_entry_avl_t *)user_data;

	printf("Timer expiry handler called for timerid-%d\n", timer_id);

	mac_entry = (mac_entry_t *)mac_node->data;

	delete_mac_entry_for_given_topo (mac_entry->mac_address,
									 mac_entry->topo_entry_ptr->topo_id);
}

/**
 * @brief add_mac_entry
 *
 * API to allow mac entry addition
 *
 * @param  avl_node_type - Avl node structure
 *
 * @return Null
 *
 */
static topo_entry_t *
add_or_lookup_topo_entry (topo_id_t topo_id,
						  const char* topo_name)
{
	topo_entry_t *topo_entry = NULL;
	avltree_t *node = NULL;
	if (topo_root != 0x0) {
		node = avltree_find(topo_root, topo_id);
	}

	if (node == NULL) {
		topo_entry = calloc(1, sizeof(topo_entry_t));
		topo_entry->topo_id = topo_id;
		strncpy(topo_entry->topo_name,
				topo_name,
				sizeof(topo_entry->topo_name));
		topo_entry->mac_tree = 0x0;
		node = avltree_create(topo_id, (void*)topo_entry);
		if (node != NULL) {
			topo_root = avltree_insert(topo_root, node);
		} else {
			printf("AVL tree node creation for topo entry has failed");
		}
	} else {
		topo_entry = (topo_entry_t *)avltree_get_data(node);
	}

	return topo_entry;
}


/**
 * @brief add_mac_entry
 *
 * API to allow mac entry addition
 *
 * @param  topo_id - topo id
 * 		   topo_name - topo name
 * 		   mac - mac address
 * 		   type - mac address type, static/dynamic
 * 		   port - port where the mac address was learnt
 *
 * @return Null
 *
 */
void
add_or_update_mac_entry (topo_id_t 		  topo_id,
						 const char		 *topo_name,
						 uchar		 	 *mac,
						 mac_entry_type_t type,
						 const char		 *port)
{
	topo_entry_t *topo_entry = NULL;
	mac_entry_t *mac_entry = NULL;
	avltree_t *node = NULL;
	uint32_t mac_key = -1;
	time_t timestamp_sec;
	size_t timer_id = 0;

	topo_entry = add_or_lookup_topo_entry(topo_id, topo_name);

	if (topo_entry) {
		mac_key = HASH_CALC(mac);
		/*
		 * MAC tree already exist and we are adding more mac entries
		 */
		if (topo_entry->mac_tree != 0x0) {
			node = avltree_find(topo_entry->mac_tree, mac_key);

			/*
			 * Mac entry node is getting updated
			 */
			if (node && avltree_get_data(node)) {
				mac_entry = (mac_entry_t *)avltree_get_data(node);
				mac_entry->topo_entry_ptr = topo_entry;
				memcpy(&mac_entry->mac_address, mac, MAC_ADDR_BYTES);
				mac_entry->type = type;
				memset(mac_entry->port, 0, sizeof(mac_entry->port));
				strncpy(mac_entry->port,
						port,
						sizeof(mac_entry->port));
				time(&timestamp_sec);
				/* remove mac node from old timer */
				remove_mac_entry_from_timer(node);
				mac_entry->learn_timestamp = timestamp_sec;
				/* add mac node to new timer */
				add_mac_entry_to_timer(node);

				return;
			}
		}
		/*
		 * We are adding new extries to mac table tree per topo
		 */
		mac_entry = calloc(1, sizeof(mac_entry_t));
		mac_entry->topo_entry_ptr = topo_entry;
		memcpy(&mac_entry->mac_address, mac, sizeof(mac_entry->mac_address));
		mac_entry->type = type;
		strncpy(mac_entry->port,
				port,
				sizeof(mac_entry->port));
		time(&timestamp_sec);
		mac_entry->learn_timestamp = timestamp_sec;

		node = avltree_create(mac_key, (void*)mac_entry);
		if (node != NULL) {
			char mac_str[MAC_ADDR_STRING];
			memset(mac_str, 0, MAC_ADDR_STRING);
			mac_print(((mac_entry_t *)avltree_get_data(node))->mac_address,
					mac_str);
			topo_entry->mac_tree = avltree_insert(topo_entry->mac_tree, node);
			printf("New entry add for topo(%u) %s "
					"%s %s %s\n", ((mac_entry_t *)avltree_get_data(node))->topo_entry_ptr->topo_id,
					mac_str,
					mac_type_to_string(mac_entry->type), mac_entry->port, ctime(&mac_entry->learn_timestamp));

			/* add mac node to new timer */
			add_mac_entry_to_timer(node);
		} else {
			printf("AVL tree node creation failed for Mac entry\n");
		}
	}

}

/* Given a binary tree, print its nodes in preorder*/
void printPreordermac(avltree_t* node)
{
	mac_entry_t *mac_entry = NULL;
	if (node == NULL)
		return;

     /* first print data of node */
    mac_entry = (mac_entry_t *)avltree_get_data(node);
    char mac_str[MAC_ADDR_STRING];
	memset(mac_str, 0, MAC_ADDR_STRING);
	mac_print(((mac_entry_t *)avltree_get_data(node))->mac_address,
			mac_str);

	printf("MAC node info:Topo(%u)- %s "
			"%s %s %s\n", mac_entry->topo_entry_ptr->topo_id,
			mac_str,
			mac_type_to_string(mac_entry->type),
			mac_entry->port,
			ctime(&mac_entry->learn_timestamp));


     /* then recur on left sutree */
	printPreordermac(node->left_child);

     /* now recur on right subtree */
	printPreordermac(node->right_child);
}

/**
 * @brief delete_mac_entry_for_given_topo
 *
 * API to allow mac entry deletion when topo id is provided.
 *
 * @param  mac - mac address
 * 		   topo_id - topo id
 *
 *
 * @return Null
 *
 */
void
delete_mac_entry_for_given_topo (uchar  *mac,
								 topo_id_t topo_id)
{
	uint32_t mac_key = -1;
	topo_entry_t *topo_entry = NULL;
	avltree_t *node = NULL;
	mac_entry_t *mac_entry = NULL;
	mac_entry_avl_t *mac_node = NULL;

	mac_key = HASH_CALC(mac);

	if (topo_root == 0x0) {
		printf("\nTopo table empty, nothing to delete\n");
		return;
	}

	if (topo_root != 0x0) {
		node = avltree_find(topo_root, topo_id);
		topo_entry = (topo_entry_t *)avltree_get_data(node);

		printf("Before delete of MAC entry for TOPO:%d, "
				"MAC table size is now=%d and preorder tree looks like->\n",
				topo_entry->topo_id,
				avltree_size(topo_entry->mac_tree));
		printPreordermac(topo_entry->mac_tree);
		printf("*******\n");

		mac_node = avltree_find(topo_entry->mac_tree, mac_key);
		if (mac_node && mac_node->data) {
			mac_entry = (mac_entry_t *)mac_node->data;
			remove_mac_entry_from_timer(mac_node);
			topo_entry->mac_tree = avltree_delete_node(topo_entry->mac_tree, mac_key);
			free(mac_entry);
		}

		/*
		 * the last mac in the mac tree associated with topo tree
		 * is deleted and hence we need to cleanup topo tree
		 */
		if (!topo_entry->mac_tree) {
			topo_id = topo_entry->topo_id;
			topo_root = avltree_delete_node(topo_root, topo_id);
			free(topo_entry);
			return;
		}
		printPreordermac(topo_entry->mac_tree);
		printf("*******\n");
	}
}

/**
 * @brief delete_mac_tree
 *
 * function to walk mac table tree and delete all the nodes
 *
 * @param  node - pointer to the node in the tree
 *
 * @return Null
 *
 */
static void
delete_mac_tree (mac_entry_avl_t *node)
{
	mac_entry_t *mac_entry = NULL;

	if (node == NULL) return;

	/* first delete both subtrees */
	delete_mac_tree(node->left_child);
	delete_mac_tree(node->right_child);

	mac_entry = (mac_entry_t *)avltree_get_data(node);
	mac_entry->topo_entry_ptr = NULL;
	stop_timer(mac_entry->timer_id);
	free(mac_entry);
	mac_entry=NULL;
	avltree_free_node(node);
}

/**
 * @brief delete_mac_entries_for_topo
 *
 * API to allow explicit mac-entry deletion for a given topo
 *
 * @param  topo_id - topo id
 *
 * @return Null
 *
 */
void
delete_mac_entries_for_topo (topo_id_t topo_id)
{
	topo_entry_t *topo_entry = NULL;
	avltree_t *node = NULL;

	if (topo_root == 0x0) {
		printf("Topo table empty, nothing to delete");
		return;
	}

	if (topo_root != 0x0) {
		node = avltree_find(topo_root, topo_id);
		topo_entry = (topo_entry_t *)avltree_get_data(node);
		printf("Delete of all MAC entries for topo-:%d\n", topo_entry->topo_id);
		/* now we will delete all mac entries associated with this topo */
		delete_mac_tree(topo_entry->mac_tree);
		topo_entry->mac_tree = NULL;
		topo_root = avltree_delete_node(topo_root, topo_id);
		free(topo_entry);
	}

}

/* Given a binary tree, print its nodes in preorder*/
void printPreorder(avltree_t* node)
{
	topo_entry_t *topo_entry = NULL;
     if (node == NULL)
          return;

     /* first print data of node */
     topo_entry = (topo_entry_t *)avltree_get_data(node);
     printf("%d %s\n", topo_entry->topo_id, topo_entry->topo_name);


     /* then recur on left sutree */
     printPreorder(node->left_child);

     /* now recur on right subtree */
     printPreorder(node->right_child);
}

