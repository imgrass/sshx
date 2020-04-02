#include <roadmap.h>
#include <ssh_sftp.h>
#include <stdlib.h>
#include <utils.h>


int main(int argc, char **argv) {
    int fd = 0;
    LIBSSH2_SESSION *session = NULL;
    struct node *node = NULL;
    struct rmt_exec_status_output status_output = {0};
    char *xml_file = argv[1];
    FILE *fp_xml = fopen(xml_file, "rb");
    if (!fp_xml) {
        ERROR("Open xml file named %s failed", xml_file);
        exit(1);
    }

    INFO("It would parse xml named %s", xml_file);
    parse_roadmap(fp_xml, &node, 0);
    print_roadmap_node_info(node);

    session = create_ssh_authed_session(&fd, &node->peer_load_info);
    get_ssh_remote_exec(session, fd, "hostname", &status_output);
    return 0;
}
