/*
 ============================================================================
 Name        : test.c
 Description : Test file for the MAC table API
 ============================================================================
 */
#include "util.h"

void add_macs_per_topo(int num_of_macs, int num_of_topos) {

    int i=0, j=0;
    uchar mac_address[MAC_ADDR_BYTES];
    char topo_name[TOPO_NAME_STRING_MAX_LEN];
    char intf_name[6];
    char mac_addr_str[MAC_ADDR_STRING];

    for (i=1; i<= num_of_topos; i++) {
        memset(topo_name, 0, sizeof(topo_name));
        memset(intf_name, 0, sizeof(intf_name));
        sprintf(topo_name, "topo_%d", i);
        sprintf(intf_name, "Fa0/%d", i);
        for (j=1; j<=num_of_macs; j++) {
            memset(mac_addr_str, 0, sizeof(mac_addr_str));
            snprintf(mac_addr_str, MAC_ADDR_STRING, "AA:AA:AA:AA:AA:%d%d", i, j);
            uchar bytes[6];
            uint8_t values[6];
            int z =0;

            if( 6 == sscanf( mac_addr_str, "%x:%x:%x:%x:%x:%x%*c",
                &values[0], &values[1], &values[2],
                &values[3], &values[4], &values[5] ) )
            {
                /* convert to uint8_t */
                for( z = 0; z < 6; ++z )
                    bytes[z] = (char) values[z];
            }

            add_or_update_mac_entry (i, topo_name, bytes, MAC_ENTRY_TYPE_DYNAMIC, intf_name);
        }
    }
}

void
update_specific_mac_entries() {

    char *mac_addr_str;
    uchar mac_address[MAC_ADDR_BYTES];

    sleep(5);
    mac_addr_str = "AA:AA:AA:AA:AA:01";
    uchar bytes[6];
    uint8_t values[6];
    int z =0;

    if( 6 == sscanf( mac_addr_str, "%x:%x:%x:%x:%x:%x%*c",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5] ) )
    {
        /* convert to uint8_t */
        for( z = 0; z < 6; ++z )
            bytes[z] = (char) values[z];
    }
    add_or_update_mac_entry (1, "topo_1", bytes, MAC_ENTRY_TYPE_DYNAMIC, "Fa0/3");

    mac_addr_str = "AA:AA:AA:AA:AA:31";
    if( 6 == sscanf( mac_addr_str, "%x:%x:%x:%x:%x:%x%*c",
            &values[0], &values[1], &values[2],
            &values[3], &values[4], &values[5] ) )
    {
        /* convert to uint8_t */
        for( z = 0; z < 6; ++z )
            bytes[z] = (char) values[z];
    }
    add_or_update_mac_entry (3, "topo_3", bytes, MAC_ENTRY_TYPE_DYNAMIC, "Fa0/11");
}

void
delete_all_mac_entries_for_topo() {

    delete_mac_entries_for_topo (3);
}

void
delete_mac_entry_for_specific_topo() {

    char *mac_addr_str;
    uchar mac_address[MAC_ADDR_BYTES];

    mac_addr_str = "AA:AA:AA:AA:AA:01";
    uchar bytes[6];
    uint8_t values[6];
    int z =0;

    if( 6 == sscanf( mac_addr_str, "%x:%x:%x:%x:%x:%x%*c",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5] ) )
    {
        /* convert to uint8_t */
        for( z = 0; z < 6; ++z )
            bytes[z] = (char) values[z];
    }
    delete_mac_entry_for_given_topo (values, 1);

}

void
delete_non_existing_mac_entry_for_specific_topo() {

    char *mac_addr_str;
    uchar mac_address[MAC_ADDR_BYTES];

    mac_addr_str = "CC:CC:CC:CC:CC:01";
    uchar bytes[6];
    uint8_t values[6];
    int z =0;

    if( 6 == sscanf( mac_addr_str, "%x:%x:%x:%x:%x:%x%*c",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5] ) )
    {
        /* convert to uint8_t */
        for( z = 0; z < 6; ++z )
            bytes[z] = (char) values[z];
    }
    delete_mac_entry_for_given_topo (values, 1);

}

/*
 * 1. Add all mac entries and wait all of them to expire
 * 2. Readd all mac entries and then update 2 mac entries, they should expire later than the rest
 * 3. a. Readd all mac entries and delete mac entry specific to topo
 *    b. Delete all mac entries of specfic topo
 *    c. Rest of the entries should expire
 * 4. Negative cases:
 * 		a. try to delete non-existing mac for non existing topo
 * 		b. try to delete non-existing mac for non-existing topo
 * 		c. try to delete non-existing mac for existing topo
 *
 */

int main(void) {
    puts("!!!Executing the various test cases as mentioned below\n"
            " * 1. Add all mac entries and wait all of them to expire.\n"
            " * 2. Readd all mac entries and then update 2 mac entries, they should expire later than the rest.\n"
            " * 3. a. Readd all mac entries and delete specific macs given mac address and topo id.\n"
            " *    b. Delete all mac entries of specfic topo.\n"
            " *    c .Rest of the entries should expire!!!");

    initialize();

    printf("\nStarting Test case 1 - Adding 5 macs each for 5 topos and waiting for them to expire\n");
    add_macs_per_topo(5, 5);
    sleep(50);


    printf("\nStarting Test case 2 - Adding 4 macs each for 3 topos, updating 2 macs and they would expire later\n");
    add_macs_per_topo(4, 3);
    update_specific_mac_entries();
    sleep(40);


    printf("\nStarting Test case 3\n"
            "a. Adding 3 macs for 3 topos, deleting an mac entry for topo 1\n"
            "b. followed by delete of all mac entries under topo 3\n"
            "c. Waiting for the rest of the macs to expire(2 macs from topo 1 and 3 macs from topo 2)\n");
    add_macs_per_topo(3, 3);
    delete_mac_entry_for_specific_topo();
    delete_all_mac_entries_for_topo();

    sleep(40);

    printf("\nStarting Test case 4\n"
                "a. try to delete non-existing mac for non existing topo\n"
                "b. try to delete all non-existing mac for non-existing topo\n"
    			"c. try to delete non-existing mac for existing topo\n");
    delete_mac_entry_for_specific_topo();
    delete_all_mac_entries_for_topo();
    add_macs_per_topo(2, 2);
    delete_non_existing_mac_entry_for_specific_topo();

    sleep(40);


    finalize();

    return EXIT_SUCCESS;
}

