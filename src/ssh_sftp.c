#include <ssh_sftp.h>


static int get_socket_fd(struct sockaddr_storage *ipaddr) {
    int fd = -1;

    if (ipaddr == NULL) {
        ERROR("Can not create socket fd due to ip address is NULL");
        return -1;
    }

    if (ipaddr->ss_family == AF_INET || ipaddr->ss_family == AF_INET6) {
        fd = socket(ipaddr->ss_family, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1) {
            ERROR("Create socket fd failed due to %s", strerror(errno));
            return fd;
        }
    } else {
        ERROR("Can not handle IP protocol <type:%u>", ipaddr->ss_family);
        return -1;
    }

    if (connect(fd, (struct sockaddr *)ipaddr, sizeof(struct sockaddr_storage))
            != 0) {
        ERROR("Failed to connect due to %s", strerror(errno));
        return -1;
    }
    return fd;
}


static void printf_fingerprint(char *fingerprint) {
    int i = 0;
    for (i=0; i<20; i++) {
        fprintf(stdout, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stdout, "\n");
    return;
}

static int authenticate_ssh_session(LIBSSH2_SESSION *session, char *user,
        char *password, char *identityfile) {
    // TODO: Add public keyfile in roadmap.xml
    char *keyfile_prv = identityfile;
    char keyfile_pub[256] = {0};
    // check what authentication methods are available
    char *userauthlist = libssh2_userauth_list(session, user, strlen(user));
    if (strstr(userauthlist, "password") && password != NULL) {
        if (libssh2_userauth_password(session, user, password)) {
            ERROR("Authentication by password failed");
            return -1;
        }
        return 0;
    }
    if (strstr(userauthlist, "publickey") && identityfile != NULL) {
        snprintf(keyfile_pub, 256, "%s.pub", keyfile_prv);
        if (libssh2_userauth_publickey_fromfile(session, user, keyfile_pub,
                    keyfile_prv, password)) {
            ERROR("Authentication by public key failed");
            return -1;
        }
        return 0;
    }
    ERROR("No supported authentication methods found!");
    return -1;
}

LIBSSH2_SESSION *create_ssh_authed_session(struct roadmap_next_node *nxt_nd) {
    int rc = 0;
    char *fingerprint = NULL;
    struct sockaddr_storage *ipaddr = get_host_from_name(nxt_nd->hostname, 22);
    int fd = get_socket_fd(ipaddr);
    LIBSSH2_SESSION *session = libssh2_session_init();
    if (!session) {
        ERROR("Could not initialize SSH session!");
        return NULL;
    }

    // TCP handshake
    rc = libssh2_session_handshake(session, fd);
    if (rc) {
        ERROR("TCP handshake failed when starting up SSH session: %d", rc);
        return NULL;
    }

    // fingerprint
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    if (fingerprint == NULL) {
        ERROR("Finger print failed!");
        return NULL;
    }
    printf_fingerprint(fingerprint);

    // authenticate SSH session
    if (authenticate_ssh_session(session, nxt_nd->user, nxt_nd->password,
            nxt_nd->identityfile) != 0) {
        ERROR("Authenticate SSH session failed!");
        return NULL;
    }
    return session;
}


char *get_ssh_remote_exec(LIBSSH2_SESSION *session, char *cmdline) {
    return NULL;
}
