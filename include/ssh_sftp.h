#ifndef __HEADER_SSH_SFTP__
#define __HEADER_SSH_SFTP__

#include <errno.h>
#include <libssh2.h>
#include <roadmap.h>
#include <string.h>
#include <sys/time.h>


/**
 * It is unknown how long the stream is, so I use the following structure to
 * store buffer.
      |<--------->| unit_len_of_sigle_buf
head->+-----------+                      <---o
      |   buf ... |    x 1                   |
      +-----------+                          |
          ...                                | num_buf_unit
tail->+-----------+-----+-----------+        |
      |   buf ... | ... |   buf ... |    x n |
      +-----------+-----+-----------+    <---o
      ^
      |-------------------> total_len
*/
struct buf_unit {
    char *buf;
    struct buf_unit *next;
};
struct rmt_exec_rslt {
    struct buf_unit *head;
    struct buf_unit *tail;
    size_t unit_len_of_sigle_buf; // suggest to set it '1k'
    size_t num_buf_unit;
    size_t total_len;
};
struct rmt_exec_status_output {
    struct variable_len_buff v_buff;
    int status_code;
};

inline void init_rmt_exec_rslt(struct rmt_exec_rslt *result) {
    result->head = NULL;
    result->tail = NULL;
    result->unit_len_of_sigle_buf = 1024; // 1k
    result->num_buf_unit = 0;
    result->total_len = 0;
}

void free_buf_in_rmt_exec_rslt(struct rmt_exec_rslt *result);

void copy_char_to_rmt_exec_rslt(char word, struct rmt_exec_rslt *result);

void copy_from_rmt_exec_rslt_to_one_buff(struct rmt_exec_rslt *result,
        struct variable_len_buff *buff);

LIBSSH2_SESSION *create_ssh_authed_session(int *fd,
        struct roadmap_next_node *nxt_nd);
int get_ssh_remote_exec(LIBSSH2_SESSION *session, int fd, char *cmdline,
        struct rmt_exec_status_output *status_output);
#endif
