#ifndef __HEADER_SSH_TUNNEL__
#define __HEADER_SSH_TUNNEL__
#include <utils.h>
#include <libssh2.h>


struct ssh_tunnel_ip_list {
    // local listen address
    struct sockaddr_storage lcl;
    // ssh server address
    struct sockaddr_storage srv;
    // remote forwarding address
    struct sockaddr_storage rmt;
};


int create_ssh_tcpip_tunnel(LIBSSH2_SESSION *session,
        struct ssh_tunnel_ip_list *stil);
#endif
