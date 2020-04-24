#include <roadmap.h>
#include <ssh_sftp.h>
#include <stdlib.h>
#include <utils.h>


static void on_exit_free_node(int status, void *node) {
    if (node != NULL) {
        INFO("++ Free callocated memory in <node:%p>", node);
        free_mem_in_node((struct node *)node);
    }
}


static void on_exit_close_ssh_session(int status, void *session) {
    if (session != NULL) {
        INFO("++ Disconnect ssh <session:%p> with peer with the <status:%d>",
                session, status);
        libssh2_session_disconnect(session, "On_exit, shutdown now");
        libssh2_session_free(session);
    }
}


static void on_exit_close_socket(int status, void *socket_fd) {
    int fd = *(int *)socket_fd;
    if (fd > 0) {
        INFO("++ Close socket <fd:%d>", fd);
        close(fd);
        libssh2_exit();
    }
}


int main(int argc, char **argv) {
    int fd = 0;
    LIBSSH2_SESSION *session = NULL;
    struct node *node = NULL;
    char *xml_file = argv[1];
    FILE *fp_xml = fopen(xml_file, "rb");

    // register on_exit functions
    on_exit(on_exit_close_socket, &fd);
    on_exit(on_exit_close_ssh_session, session);
    on_exit(on_exit_free_node, node);

    if (!fp_xml) {
        ERROR("Open xml file named %s failed", xml_file);
        exit(1);
    }

    INFO("It would parse xml named %s", xml_file);
    parse_roadmap(fp_xml, &node, 0);
    print_roadmap_node_info(node);

    session = create_ssh_authed_session(&fd, &node->peer_load_info);
    if (session==NULL) {
        ERROR("Create SSH session failed, exit...");
        return 1;
    }

    /*
    // ** test execute cmd in remote host...
    struct rmt_exec_status_output status_output = {0};
    char *cmdline = argv[2];
    get_ssh_remote_exec(session, fd, cmdline, &status_output);
    INFO("status code is %d, and output is:", status_output.status_code);
    PRINTF_STDOUT_BUFF(status_output.vl.buff, status_output.vl.size);
    */

    /*
    // ** test download file from remote host...
    char *remote_path = argv[2];
    char *local_path = argv[3];
    scp_download_one_non_blocking(session, fd, remote_path, local_path);
    */

    // ** test upload file to remote host...
    char *local_path = argv[2];
    char *remote_path = argv[3];
    scp_upload_one_non_blocking(session, fd, local_path, remote_path);

    return 0;
}
