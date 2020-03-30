#include <roadmap.h>
#include <stdlib.h>
#include <utils.h>


int main(int argc, char **argv) {
    struct node *node = NULL;
    char *xml_file = argv[1];
    FILE *fp_xml = fopen(xml_file, "rb");
    if (!fp_xml) {
        ERROR("Open xml file named %s failed", xml_file);
        exit(1);
    }

    INFO("It would parse xml named %s", xml_file);
    parse_roadmap(fp_xml, &node, 0);
    print_roadmap_node_info(node);
    return 0;
}
