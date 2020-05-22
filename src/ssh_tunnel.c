/**
            TCP/IP forwarding schematic diagram

        +----------------------------------------+
        |                tunnel                  |
        +----------------------------------------+
        H                                        H
  +-----H----------+         +-------------------H------+
  |     H          |         |                   H      |
  |   +---+        |         |    +---+        +---+    |
  |   |   |---------------------->|   |------->|   |    |
  |   +---+        |         |    +---+        +---+    |
  | lcl_lstn_ip    |         |   srv_ip       rmt_ip    |
  | lcl_lstn_port  |         |   srv_port     rmt_port  |
  |                |         |                          |
  |   client       |         |        server            |
  +----------------+         +--------------------------+
                    (figure 1)
*/
#include <ssh_tunnel.h>


static int printf_ipsock(struct sockaddr_storage *ipsock, char *description) {
    struct ipstr ip_str = {0};

    PRINTF_STDOUT_BEAUTIFULLY(0, "%s", description);
    if (ip2str(&ip_str, ipsock) != 0) {
        return 1;
    }
    PRINTF_STDOUT_BEAUTIFULLY(1, "ver  -> <%d>", ip_str.version);
    PRINTF_STDOUT_BEAUTIFULLY(1, "ip   -> <%s>", ip_str.addr);
    PRINTF_STDOUT_BEAUTIFULLY(1, "port -> <%d>", ip_str.port);
    return 0;
}

static int check_and_printf_stil(struct ssh_tunnel_ip_list *stil) {
    int rc = 0;
    INFO("Print ssh TCP/IP tunnel ip list:");
    rc |= printf_ipsock(&stil->lcl, "Local listening address");
    rc |= printf_ipsock(&stil->srv, "Ssh server address");
    rc |= printf_ipsock(&stil->rmt, "Remote forwarding address");
    return rc;
}


int create_ssh_tcpip_tunnel(LIBSSH2_SESSION *session,
        struct ssh_tunnel_ip_list *stil) {
    int api_block_mode = libssh2_session_get_blocking(session);
    int listensock = 0, forwardsock = 0;

    if (check_and_printf_stil(stil) != 0) {
        ERROR("IP address is invalid!");
        return 1;
    }

    // set session non-blocking
    libssh2_session_set_blocking(session, 0);

    // create local listening socket
    
    return 0;
}
