#ifndef __HEADER_UTILS__
#define __HEADER_UTILS__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


#define INFO(...) do { \
    fprintf(stdout, "+[INFO][Line: %u] ", __LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
} while(0);

#define ERROR(...) do { \
    fprintf(stderr, "-[ERROR][Line: %u] ", __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while(0);

#ifdef IMGRASS_DEBUG
#define DEBUG(...) do { \
    fprintf(stdout, "-[DEBUG][Line: %u] ", __LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
} while(0);

#else
#define DEBUG(...)
#endif

#define PRINTF_STDOUT_BEAUTIFULLY(index, ...) do {\
    int i = 0; \
    fprintf(stdout, "*"); \
    while (i++<index) { \
        fprintf(stdout, "    "); \
    } \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
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
