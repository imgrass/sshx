#ifndef __HEADER_ROADMAP__
#define __HEADER_ROADMAP__

#include <unistd.h>
#include <utils.h>
#include <xml.h>


#define SAFE_COPY_FROM_XML_TO_STR(str, xml_str) do {\
    (str) = (char *)calloc(xml_string_length((xml_str))+1, sizeof(char)); \
    if (!str) { \
        ERROR("Allocate memory failed") \
    } \
    xml_string_copy((xml_str), (uint8_t *)(str), \
            xml_string_length((xml_str))); \
} while(0);
#define SAFE_FREE_STR_MEMORY(str) do {\
    free(str); \
} while(0);

#ifdef IMGRASS_DEBUG
#else
#endif


/**
 * TODO: The information of all nodes will be summarized later, and struct
 * 'node' will be turned into a linked list to comfort management.
 */
struct roadmap_next_node {
    char *hostname;
    char *user;
    char *password;
    char *identityfile;
};

struct local_node_info {
    char hostname[64];
    // ...
};

struct node {
    struct local_node_info local_info;
    struct roadmap_next_node peer_load_info;

    int layer;
    // int is_local_node;
    // struct node *next;
};


int parse_roadmap(FILE *source, struct node **node, int layer);
void print_roadmap_node_info(struct node *node);
#endif
