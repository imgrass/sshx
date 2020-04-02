#ifndef __HEADER_SSH_SFTP__
#define __HEADER_SSH_SFTP__

#include <errno.h>
#include <libssh2.h>
#include <roadmap.h>
#include <sshx_string.h>
#include <string.h>
#include <sys/time.h>

struct rmt_exec_status_output {
    struct vl_buff vl;
    int status_code;
};

LIBSSH2_SESSION *create_ssh_authed_session(int *fd,
        struct roadmap_next_node *nxt_nd);
int get_ssh_remote_exec(LIBSSH2_SESSION *session, int fd, char *cmdline,
        struct rmt_exec_status_output *status_output);
#endif
