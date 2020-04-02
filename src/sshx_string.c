#include <sshx_string.h>


void free_mem_of_ukl_buff(struct ukl_buff *ukl) {
    struct ukl_buf_unit *ptr = NULL;
    while ((ptr=ukl->head)!=NULL) {
        ukl->head = ptr->next;
        free(ptr->buf);
        free(ptr);
    }
}


void add_word_to_ukl_buff(int8_t word, struct ukl_buff *ukl) {
    int n = ukl->num_buf_unit, u = ukl->unit_len_of_single_buf,
        t = ukl->total_len;
    int rest = t - (n * (n - 1) / 2) * u;
    struct ukl_buf_unit *ptr = NULL;

    if (rest<n*u) {
        ukl->tail->buf[rest] = word;
    } else if (rest==0 || rest==n*u) {
        ptr = (struct ukl_buf_unit *)calloc(1, sizeof(struct ukl_buf_unit));
        ptr->buf = (int8_t *)calloc(1, (n+1)*u);
        ptr->buf[0] = word;
        ptr->next = NULL;

        // Insert first word to buff, it need to specify 'head'
        if (rest==0) {
            ukl->head = ptr;
            ukl->tail = ptr;
        } else {
            ukl->tail->next = ptr;
            ukl->tail = ptr;
        }
        ukl->num_buf_unit++;
    }
    ukl->total_len++;
}


void copy_from_ukl_buff_to_vl_buff(struct ukl_buff *ukl, struct vl_buff *vl) {
    struct ukl_buf_unit *ptr_buf_unit = ukl->head;
    int8_t *ptr_word = NULL;
    size_t i = 0, offset = 0, rest = 0;

    // clear vl_buff
    vl->buff = NULL;
    vl->size = 0;
    if ((rest=ukl->total_len)==0) {
        DEBUG("The buff of ukl_buff is NULL");
        return;
    }

    vl->size = ukl->total_len;
    vl->buff = (int8_t *)calloc(1, vl->size);

    ptr_word = vl->buff;
    while (1) {
        if (ptr_buf_unit == NULL) {
            break;
        }
        i++;

        if (i < ukl->num_buf_unit) {
            offset = i * ukl->unit_len_of_single_buf;
            memcpy(ptr_word, ptr_buf_unit->buf, offset);
            ptr_word = ptr_word + offset;
            rest -= offset;
        } else if (i == ukl->num_buf_unit) {
            memcpy(ptr_word, ptr_buf_unit->buf, rest);
        } else {
            ERROR("The format of rmt_exec_rslt is error!!! Pls check it");
            exit(1);
        }

        ptr_buf_unit = ptr_buf_unit->next;
    }
}
