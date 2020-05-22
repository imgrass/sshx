#include <utils.h>


int get_host_from_name(struct sockaddr_storage *ipaddr, char *hostname) {
    struct addrinfo hints = {0}, *res = NULL, *first_res = NULL;
    int rc = 0;

    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    rc = getaddrinfo(hostname, NULL, &hints, &res);
    if (rc != 0) {
        ERROR("Can not get addr from <hostname:%s>", hostname);
        freeaddrinfo(res);
        return 1;
    }
    first_res = res;

#ifdef IMGRASS_DEBUG
    struct ipstr ip_str = {0};
    DEBUG("==> Get addr from <hostname:%s>", hostname);
    while (res) {
        rc = ip2str(&ip_str, (struct sockaddr_storage *)(res->ai_addr));
        if (rc==1) {
            PRINTF_RAW_STRING(stdout, "*!  invalid ip address\n");
            continue;
        }
        PRINTF_RAW_STRING(stdout, "*o  IPv%d %s\n", ip_str.version, ip_str.addr);
        res = res->ai_next;
    }
#else
#endif
    memcpy(ipaddr, (struct sockaddr_storage *)(first_res->ai_addr),
            sizeof(struct sockaddr_storage));
    freeaddrinfo(first_res);
    return 0;
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


int str2ip(struct sockaddr_storage *ip, char *ipstr, uint16_t port) {
    int rc = 0;

    // check if ipstr is ipv4
    rc = inet_pton(AF_INET, ipstr, &((struct sockaddr_in *)ip)->sin_addr);
    if (rc==1) {
        ip->ss_family = AF_INET;
        ((struct sockaddr_in *)ip)->sin_port = htons(port);
        DEBUG("ipstr:<%s> is ipv4 address\n", ipstr);
        return 0;
    }

    // check if ipstr is ipv6
    rc = inet_pton(AF_INET6, ipstr, &((struct sockaddr_in6 *)ip)->sin6_addr);
    if (rc==1) {
        ip->ss_family = AF_INET6;
        ((struct sockaddr_in6 *)ip)->sin6_port = htons(port);
        DEBUG("ipstr:<%s> is ipv6 address\n", ipstr);
        return 0;
    }

    DEBUG("ipstr:<%s> is invalid ip address\n", ipstr);
    return 1;
}


int embed_port(struct sockaddr_storage *ip, uint16_t port) {
    if (ip->ss_family==AF_INET) {
        ((struct sockaddr_in *)ip)->sin_port = htons(port);
        return 0;
    } else if (ip->ss_family==AF_INET6) {
        ((struct sockaddr_in6 *)ip)->sin6_port = htons(port);
        return 0;
    } else {
        return 1;
    }
}


int ip2str(struct ipstr *ip_str, struct sockaddr_storage *ip_sock) {
    if (ip_sock->ss_family == AF_INET) {
        ip_str->version = 4;
        inet_ntop(AF_INET, &((struct sockaddr_in *)ip_sock)->sin_addr,
                ip_str->addr, INET6_ADDRSTRLEN);
        ip_str->port = ntohs(((struct sockaddr_in *)ip_sock)->sin_port);
        return 0;
    } else if (ip_sock->ss_family == AF_INET6) {
        ip_str->version = 6;
        inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ip_sock)->sin6_addr,
                ip_str->addr, INET6_ADDRSTRLEN);
        ip_str->port = ntohs(((struct sockaddr_in6 *)ip_sock)->sin6_port);
        return 0;
    }
    return 1;
}
