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


struct sockaddr_storage *get_host_from_name(char *hostname) {
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
    return (struct sockaddr_storage *)(first_res->ai_addr);
}
