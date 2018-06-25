#include "util.h"

#define UINT8_MAX_VAL 0xFF

void
increment_mac_address(uint8_t *mac, uint32_t inc) {
    while (inc) {
        uint8_t index = 5;
        if (mac[5] < UINT8_MAX_VAL - 1) {
            index = 5;
        } else if (mac[4] < UINT8_MAX_VAL - 1) {
            index = 4;
        } else if (mac[3] < UINT8_MAX_VAL - 1) {
            index = 3;
        } else if (mac[2] < UINT8_MAX_VAL - 1) {
            index = 2;
        } else if (mac[1] < UINT8_MAX_VAL - 1) {
            index = 1;
        } else if (mac[0] < UINT8_MAX_VAL - 1) {
            index = 0;
        }
        mac[index] = (mac[index]+1) % UINT8_MAX_VAL;
        inc--;
    }
}

char*
mac_type_to_string (mac_entry_type_t type)
{
    switch (type) {
    case MAC_ENTRY_TYPE_INVALID:
        return "Invalid";
    case MAC_ENTRY_TYPE_DYNAMIC:
        return "DYNAMIC";
    case MAC_ENTRY_TYPE_STATIC:
        return "STATIC";
    default:
        break;
    }
    return "unknown";
}

/* UTILITY FUNCTIONS */
/* Function to push an item to sNode*/
void push(struct sNode** top_ref, topo_avl_node_t *t)
{
	/* allocate tNode */
	struct sNode* new_tNode =
			(struct sNode*) malloc(sizeof(struct sNode));

	if(new_tNode == NULL)
	{
		printf("Stack Overflow \n");
		getchar();
		exit(0);
	}

	/* put in the data  */
	new_tNode->t  = t;

	/* link the old list off the new tNode */
	new_tNode->next = (*top_ref);

	/* move the head to point to the new tNode */
	(*top_ref)    = new_tNode;
}

/* The function returns true if stack is empty, otherwise false */
bool isEmpty(struct sNode *top)
{
	return (top == NULL)? 1 : 0;
}

/* Function to pop an item from stack*/
topo_avl_node_t *pop(struct sNode** top_ref)
{
	topo_avl_node_t *res;
	struct sNode *top;

	/*If sNode is empty then error */
	if(isEmpty(*top_ref))
	{
		printf("Stack Underflow \n");
		getchar();
		exit(0);
	}
	else
	{
		top = *top_ref;
		res = top->t;
		*top_ref = top->next;
		free(top);
		return res;
	}
}

void mac_print(uchar *mac, char *mac_str){
	int values[6];
	int z =0;

	for( z = 0; z < 6; z++ ) {
		values[z] = (int) mac[z];
	}

	snprintf(mac_str, MAC_ADDR_STRING, "%x:%x:%x:%x:%x:%x",
		values[0], values[1], values[2],
		values[3], values[4], values[5] );
}
