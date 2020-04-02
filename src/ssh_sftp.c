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


static void printf_fingerprint(const char *fingerprint) {
    int i = 0;
    fprintf(stdout, "\n+ fingerprint:\n");
    for (i=0; i<20; i++) {
        fprintf(stdout, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stdout, "\n\n");
    return;
}

static int authenticate_ssh_session(LIBSSH2_SESSION *session, char *user,
        char *password, char *identityfile) {
    int rc = 0;
    // TODO: Add public keyfile in roadmap.xml
    char *keyfile_prv = identityfile;
    char keyfile_pub[256] = {0};
    // check what authentication methods are available
    char *userauthlist = libssh2_userauth_list(session, user, strlen(user));
    if (strstr(userauthlist, "password") && password != NULL) {
        while ((rc=libssh2_userauth_password(session, user, password))==
                LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            ERROR("Authentication by password failed due to %s",
                    strerror(errno));
        } else {
            DEBUG("Authenticate SSH session with password successfully!");
            return 0;
        }
    }
    if (strstr(userauthlist, "publickey") && identityfile != NULL) {
        snprintf(keyfile_pub, 256, "%s.pub", keyfile_prv);
        while ((rc=libssh2_userauth_publickey_fromfile(session, user,
                keyfile_pub, keyfile_prv, password))==LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            ERROR("Authentication by public key failed due to %s",
                    strerror(errno));
        } else {
            DEBUG("Authenticate SSH session with identityfile successfully!");
            return 0;
        }
    }
    ERROR("No supported authentication methods found!");
    return -1;
}

LIBSSH2_SESSION *create_ssh_authed_session(int *fd,
        struct roadmap_next_node *nxt_nd) {
    int rc = 0;
    const char *fingerprint = NULL;
    struct sockaddr_storage *ipaddr = get_host_from_name(nxt_nd->hostname, 22);
    *fd = get_socket_fd(ipaddr);
    LIBSSH2_SESSION *session = libssh2_session_init();
    if (!session) {
        ERROR("Could not initialize SSH session!");
        return NULL;
    }

    INFO("ssh %s@%s -p %s", nxt_nd->user, nxt_nd->hostname, "22")
    // TCP handshake
    rc = libssh2_session_handshake(session, *fd);
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


static int wait_socket(int socket_fd, LIBSSH2_SESSION *session) {
    struct timeval timeout = {0};
    int rc = 0;
    fd_set fd = {0};
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int direction = 0;

    // timeout is set to 10s.
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);
    FD_SET(socket_fd, &fd);

    // make sure it wait in the correct direction
    direction = libssh2_session_block_directions(session);
    if (direction & LIBSSH2_SESSION_BLOCK_INBOUND) {
        writefd = &fd;
    }
    rc = select(socket_fd+1, readfd, writefd, NULL, &timeout);
    return rc;
}


static void read_output_from_channel(LIBSSH2_SESSION *session,
        LIBSSH2_CHANNEL *channel, int socket_fd, struct ukl_buff *result) {
    int i = 0, rc = 0;
    size_t buff_size = 0x4000;
    char buff[buff_size];
    while (1) {
        rc = 0;
        do {
            bzero(buff, buff_size);
            rc = libssh2_channel_read(channel, buff, buff_size);
            if (rc > 0) {
                for (i=0; i<rc; i++) {
                    add_word_to_ukl_buff(buff[i], result);
                }
            } else {
                if (rc != LIBSSH2_ERROR_EAGAIN) {
                    // DEBUG("Libssh2_channel_read returned %d", rc);
                    ;;
                }
            }
        } while (rc > 0);

        // blocking may occurs, so we should loop in this condition.
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            wait_socket(socket_fd, session);
        } else {
            break;
        }
    }
}


int get_ssh_remote_exec(LIBSSH2_SESSION *session, int fd, char *cmdline,
        struct rmt_exec_status_output *status_output) {
    int rc = 0;
    char *exitsignal = (char *)"none";
    LIBSSH2_CHANNEL *channel = NULL;
    struct ukl_buff result = {0};
    init_ukl_buff(&result, 0);

    INFO("Exec cmd: <%s>", cmdline);
    // exec Non-Blocking on the remote host
    // * open channel
    while ((channel=libssh2_channel_open_session(session))==NULL &&
            libssh2_session_last_error(session, NULL, NULL, 0)==
            LIBSSH2_ERROR_EAGAIN) {
        wait_socket(fd, session);
    }
    if (channel==NULL) {
        ERROR("exec cmd:<%s> in peer host failed due to open channel failed,"
                "the detailed reason is: %s", cmdline,
                strerror(errno));
        return -1;
    }
    DEBUG("Open channel successfully!");

    // * exec cmd in peer host
    while ((rc=libssh2_channel_exec(channel, cmdline))==LIBSSH2_ERROR_EAGAIN) {
        wait_socket(fd, session);
    }
    if (rc) {
        ERROR("exec cmd:<%s> in peer host failed due to %s", cmdline,
                strerror(errno));
        return -1;
    }
    DEBUG("Exec cmd successfully!");

    // * get result after the command was run in peer host
    read_output_from_channel(session, channel, fd, &result);
    copy_from_ukl_buff_to_vl_buff(&result, &status_output->vl);
    free_mem_of_ukl_buff(&result);
    DEBUG("Get output successfully!");

    // * get exitcode
    status_output->status_code = 127;
    while ((rc=libssh2_channel_close(channel))==LIBSSH2_ERROR_EAGAIN) {
        wait_socket(fd, session);
    }
    if (rc==0) {
        status_output->status_code = libssh2_channel_get_exit_status(channel);
        libssh2_channel_get_exit_signal(channel, &exitsignal, NULL, NULL, NULL,
                NULL, NULL);
    }
    if (exitsignal) {
        INFO("Got signal from peer: %s", exitsignal);
    } else {
        INFO("EXIT: %d; Totally get %lu bytes", status_output->status_code,
                status_output->vl.size);
    }
    return 0;
}
