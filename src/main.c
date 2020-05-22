#include <roadmap.h>
#include <ssh_sftp.h>
#include <ssh_tunnel.h>
#include <stdlib.h>
#include <utils.h>


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
    struct node nd = {0};
    char *xml_file = argv[1];
    FILE *fp_xml = fopen(xml_file, "rb");

    // register on_exit functions
    on_exit(on_exit_close_socket, &fd);
    on_exit(on_exit_close_ssh_session, session);

    if (!fp_xml) {
        ERROR("Open xml file named %s failed", xml_file);
        exit(1);
    }

    INFO("It would parse xml named %s", xml_file);
    if (parse_roadmap(fp_xml, &nd, 0) != 0) {
        ERROR("Parse roadmap failed!");
        exit(1);
    }
    print_roadmap_node_info(&nd);

    session = create_ssh_authed_session(&fd, &nd.peer_load_info);
    if (session==NULL) {
        ERROR("Create SSH session failed, exit...");
        exit(1);
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

    /*
    // ** test upload file to remote host...
    char *local_path = argv[2];
    char *remote_path = argv[3];
    scp_upload_one_non_blocking(session, fd, local_path, remote_path);
    */

    /*
    // local listen address
    struct sockaddr_storage lcl;
    // ssh server address
    struct sockaddr_storage srv;
    // remote forwarding address
    struct sockaddr_storage rmt;
    */

    // ** test ip forward
    int rc = 0;
    struct ssh_tunnel_ip_list stil = {0};
    memcpy(&stil.srv, &nd.peer_load_info.ipaddr,
            sizeof(struct sockaddr_storage));

    rc |= str2ip(&stil.lcl, "127.0.0.1", 25535);
    rc |= str2ip(&stil.rmt, "127.0.0.1", 25535);
    if (rc != 0) {
        ERROR("Init ssh_tunnel_ip_list failed");
        exit(1);
    }
    create_ssh_tcpip_tunnel(session, &stil);

    return 0;
}
