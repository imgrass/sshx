#include <utils.h>


void printf_ipaddr(struct sockaddr_storage *ipaddr) {
    char addrstr[64] = {0};
    void *ptr = NULL;
    switch (ipaddr->ss_family) {
        case AF_INET:
            ptr = &((struct sockaddr_in *)ipaddr)->sin_addr;
            break;
        case AF_INET6:
            ptr = &((struct sockaddr_in6 *)ipaddr)->sin6_addr;
            break;
    }
    inet_ntop(ipaddr->ss_family, ptr, addrstr, 64);
    INFO("IPv%d address: %s", ipaddr->ss_family == PF_INET6 ? 6 : 4,
            addrstr);
    return;
}


struct sockaddr_storage *get_host_from_name(char *hostname,
        unsigned short port) {
    struct addrinfo hints = {0}, *res = NULL, *first_res = NULL;
    int errcode = 0;

    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo(hostname, NULL, &hints, &res);
    if (errcode != 0) {
        ERROR("Can not get addr from <hostname:%s>", hostname);
        return NULL;
    }
    first_res = res;

#ifdef IMGRASS_DEBUG
    char addrstr[64] = {0};
    void *ptr = NULL;
    DEBUG("==> Get addr from <hostname:%s>", hostname);
    while (res) {
        switch (res->ai_family) {
            case AF_INET:
                ptr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
                break;
        }
        inet_ntop(res->ai_family, ptr, addrstr, 64);
        DEBUG("IPv%d address: %s", res->ai_family == PF_INET6 ? 6 : 4,
                addrstr);
        res = res->ai_next;
    }
#else
#endif
    // Add port here.
    if (first_res->ai_family == AF_INET) {
        ((struct sockaddr_in *)(first_res->ai_addr))->sin_port = htons(port);
    } else if (first_res->ai_family == AF_INET6) {
        ((struct sockaddr_in6 *)(first_res->ai_addr))->sin6_port = htons(port);
    }
    return (struct sockaddr_storage *)(first_res->ai_addr);
}


struct vl_buff *format_multiline(int8_t *mline, size_t len, char *prefix) {
    size_t nline = 0;
    int8_t *pcur=NULL, *pnew=NULL, *pbuf=NULL;
    size_t buf_size = 0;
    size_t prefix_size = strlen(prefix);
    struct vl_buff *vl = NULL;
    int8_t *buf = NULL;

    // count numbers of all '\n' in mline
    for (pcur=mline; pcur-mline<len; pcur++) {
        if (*pcur=='\n') {
            nline++;
        }
    }
    nline++;

    vl = (struct vl_buff *)calloc(1, sizeof(struct vl_buff));
    buf_size = nline * prefix_size + len + 1;
    buf = (int8_t *)calloc(1, buf_size);
    vl->buff = buf;
    vl->size = buf_size;

    for (pcur=mline, pnew=mline, pbuf=buf;; pnew++) {
        if (*pnew=='\n' || pnew-mline==len) {
            memcpy(pbuf, prefix, prefix_size);
            pbuf += prefix_size;

            if (pnew-mline==len) {
                memcpy(pbuf, pcur, pnew-pcur);
                pbuf += (pnew - pcur);
                break;
            } else {
                memcpy(pbuf, pcur, pnew-pcur+1);
                pbuf += (pnew - pcur + 1);
                pcur = pnew + 1;
            }
        }
    }
    memcpy(pbuf, "\n", 1);
    return vl;
}


void _printf_buff(int8_t *buf, size_t size, char *prefix, int fd) {
    struct vl_buff *vl = NULL;
    vl = format_multiline(buf, size, prefix);
    if (write(fd, vl->buff, vl->size) == -1) {
        ERROR("Failed to write buffer to fd:%d due to %s", fd,
                strerror(errno));
    }
    free_vl_buff(vl);
}
