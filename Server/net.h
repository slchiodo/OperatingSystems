
#ifndef _NET_H_
#define _NET_H_

#include <netdb.h> 
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "logging.h"

enum msg_types {
	MSG_READDIR = 1,
	MSG_OPEN = 2,
	MSG_READ = 3,
	MSG_GETATTR = 4,
};

struct __attribute__((__packed__)) netfs_msg_header {
    uint64_t msg_len;
    uint16_t msg_type;
};

int connect_to (char *hostname, int port);
ssize_t write_len (int fd, void *buf, size_t length);
ssize_t read_len (int fd, void *buf, size_t length);

#endif