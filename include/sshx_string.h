#ifndef __HEADER_SSHX_STRING__
#define __HEADER_SSHX_STRING__

#include <string.h>
#include <utils.h>
/**
 * It is unknown how long the stream is, so I use the following structure to
 * store buffer <rmt_exec_rslt>
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

     ..........................................
                     | |
                     | |  combine to a single buff
                     v v      <rmt_exec_status_output>
      +-------------------------------------+
      |    buf1   buf2   ...   bufn         |
      +-------------------------------------+
*/
struct ukl_buf_unit {
    int8_t *buf;
    struct ukl_buf_unit *next;
};
struct ukl_buff {   // Unknown length buffer
    struct ukl_buf_unit *head;
    struct ukl_buf_unit *tail;
    size_t unit_len_of_single_buf; // suggest to set it '1k'
    size_t num_buf_unit;
    size_t total_len;
};
struct vl_buff {    // Variable length buffer
    int8_t *buff;
    size_t size;
};


inline void init_ukl_buff(struct ukl_buff *ukl, size_t unit_len) {
    ukl->head = NULL;
    ukl->tail = NULL;
    // default unit length is 1k
    ukl->unit_len_of_single_buf = (unit_len==0?1024:unit_len);
    ukl->num_buf_unit = 0;
    ukl->total_len = 0;
}


inline void free_mem_of_vl_buff(struct vl_buff *vl) {
    free(vl->buff);
}


void free_mem_of_ukl_buff(struct ukl_buff *ukl);
void add_word_to_ukl_buff(int8_t word, struct ukl_buff *ukl);
void copy_from_ukl_buff_to_vl_buff(struct ukl_buff *ukl, struct vl_buff *vl);
void printf_ukl_buff(struct ukl_buff *ukl);
#endif
