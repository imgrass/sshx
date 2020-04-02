#ifndef __HEADER_UTILS__
#define __HEADER_UTILS__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LOG_BASE(type, fd, ...) do { \
    size_t len = strlen(__FILE__); \
    char *file_name = __FILE__, *ptr = NULL; \
    for (ptr=file_name+len; *ptr!='/' && ptr>=file_name; ptr--); \
    fprintf((fd), "+[%s][%s::%s][Line: %u] ", (type),  ptr+1,  __func__, \
            __LINE__); \
    fprintf((fd), __VA_ARGS__); \
    fprintf((fd), "\n"); \
} while(0);

#define INFO(...) LOG_BASE("INFO", stdout, __VA_ARGS__)

#define ERROR(...) LOG_BASE("ERROR", stderr, __VA_ARGS__)

#ifdef IMGRASS_DEBUG
#define DEBUG(...) LOG_BASE("DEBUG", stdout, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define PRINTF_STDOUT_BEAUTIFULLY(index, ...) do { \
    int i = 0; \
    fprintf(stdout, "*"); \
    while (i++<index) { \
        fprintf(stdout, "    "); \
    } \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
} while(0);

#define PRINTF_STDOUT_BUFF(buf, size) do { \
    size_t i; \
    fprintf(stdout, "<"); \
    for (i=0; i<(size); i++) { \
        fprintf(stdout, "%c", (char)(buf)[i]); \
    } \
    fprintf(stdout, ">\n"); \
} while(0);

/**
 * size = 1 + 1, the first '1' is reserved for '\0' of string, and another
 * is used when num is 0.
 */
#define INT_TO_STR(num, str) do { \
    int size = 1 + 1; \
    for (; (num)>0; (num)/=10, size++); \
    (str) = (char *)calloc(size, sizeof(char)); \
    snprintf((str), size, "%d%c", (num), 0); \
} while(0);


void printf_ipaddr(struct sockaddr_storage *ipaddr);
struct sockaddr_storage *get_host_from_name(char *hostname,
        unsigned short port);
#endif
