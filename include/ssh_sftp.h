#ifndef __HEADER_SSH_SFTP__
#define __HEADER_SSH_SFTP__

#include <libssh2.h>
#include <roadmap.h>


LIBSSH2_SESSION *create_ssh_session(struct roadmap_next_node *nxt_nd);
char *get_ssh_remote_exec(LIBSSH2_SESSION *session, char *cmdline);

#endif
