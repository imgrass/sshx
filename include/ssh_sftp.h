#ifndef __HEADER_SSH_SFTP__
#define __HEADER_SSH_SFTP__

#include <errno.h>
#include <libssh2.h>
#include <roadmap.h>
#include <string.h>


LIBSSH2_SESSION *create_ssh_authed_session(struct roadmap_next_node *nxt_nd);
char *get_ssh_remote_exec(LIBSSH2_SESSION *session, char *cmdline);

#endif
