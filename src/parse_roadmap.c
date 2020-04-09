#include <roadmap.h>


static int string_equals(struct xml_string* xml_str, char const* str) {
    size_t len_xml_str = xml_string_length(xml_str);
    size_t len_str = strlen(str);
    size_t i = 0;

    char* xml_str_buff = calloc(len_xml_str + 1, sizeof(char));
    xml_string_copy(xml_str, (uint8_t *)xml_str_buff, len_xml_str);
    xml_str_buff[len_xml_str] = 0;

    if (len_xml_str != len_str) {
        return 0;
    }

    for (i=0; i<len_xml_str; ++i) {
        if (xml_str_buff[i] != str[i]) {
            return 0;
        }
    }
    return 1;
}


static int fill_node_from_source(struct node **node, struct xml_node *xml_node,
        int layer) {
    *node = (struct node*)calloc(1, sizeof(struct node));
    if (!*node) {
        ERROR("Allocate memory for struct node failed");
        exit(1);
    }

    (*node)->layer = layer;
    SAFE_COPY_FROM_XML_TO_STR((*node)->peer_load_info.hostname,
            xml_node_content(xml_easy_child(xml_node,
                    (uint8_t *)"Hostname", 0)))
    SAFE_COPY_FROM_XML_TO_STR((*node)->peer_load_info.user,
            xml_node_content(xml_easy_child(xml_node,
                    (uint8_t *)"User", 0)))
    SAFE_COPY_FROM_XML_TO_STR((*node)->peer_load_info.password,
            xml_node_content(xml_easy_child(xml_node,
                    (uint8_t *)"Password", 0)))
    SAFE_COPY_FROM_XML_TO_STR((*node)->peer_load_info.identityfile,
            xml_node_content(xml_easy_child(xml_node,
                    (uint8_t *)"IdentityFile", 0)))

    gethostname((*node)->local_info.hostname, 64);
    return 0;
}


int parse_roadmap(FILE *source, struct node **node, int layer) {
    int idx_node = 0;
    int idx_attr = 0;
    int flg_lcl_nd_is_found = 0;
    int len_attr = 0;
    char *str_layer = NULL;
    struct xml_node *nd_rt = NULL;
    struct xml_node *nd_lcl = NULL;
    struct xml_document *document = xml_open_document(source);
    if (!document) {
        ERROR("Could not parse document\n");
        exit(1);
    }

    nd_rt = xml_document_root(document);
    if (!string_equals(xml_node_name(nd_rt), "RoadMap")) {
        ERROR("Root name of xml must be <RoadMap>");
    }

    INT_TO_STR(layer, str_layer);
    while ((nd_lcl=xml_node_child(nd_rt, idx_node++))) {
        len_attr = xml_node_attributes(nd_lcl);
        if (!string_equals(xml_node_name(nd_lcl), "Node")) {
            continue;
        }
        for (idx_attr=0; idx_attr<len_attr; idx_attr++) {
            if (string_equals(xml_node_attribute_name(nd_lcl, idx_attr),
                        "layer") &&
                string_equals(xml_node_attribute_content(nd_lcl, idx_attr),
                        str_layer)) {
                flg_lcl_nd_is_found = 1;
                break;
            }
        }
        if (flg_lcl_nd_is_found) {
            break;
        }
    }

    if (!flg_lcl_nd_is_found) {
        ERROR("Can not found node with <layer:%d> in roadmap", layer);
        exit(1);
    }

    fill_node_from_source(node, nd_lcl, layer);
    xml_document_free(document, true);
    return 0;
}


void print_roadmap_node_info(struct node *node) {
    PRINTF_STDOUT_BEAUTIFULLY(0, "Node");
    PRINTF_STDOUT_BEAUTIFULLY(1, "local info");
    PRINTF_STDOUT_BEAUTIFULLY(2, "hostname -> <%s>",
            node->local_info.hostname);
    PRINTF_STDOUT_BEAUTIFULLY(1, "peer_load info");
    PRINTF_STDOUT_BEAUTIFULLY(2, "hostname -> <%s>",
            node->peer_load_info.hostname);
    PRINTF_STDOUT_BEAUTIFULLY(2, "user -> <%s>",
            node->peer_load_info.user);
    PRINTF_STDOUT_BEAUTIFULLY(2, "password -> <%s>",
            node->peer_load_info.password);
    PRINTF_STDOUT_BEAUTIFULLY(2, "identityfile -> <%s>",
            node->peer_load_info.identityfile);
}


void free_mem_in_node(struct node *node) {
    struct roadmap_next_node *nd = &node->peer_load_info;
    free(nd->hostname);
    free(nd->user);
    free(nd->password);
    free(nd->identityfile);
}
