/**
 * netfs_client.h
 *
 * Implementation of the netfs client file system. Based on the fuse 'hello'
 * example here: https://github.com/libfuse/libfuse/blob/master/example/hello.c
 */

#define FUSE_USE_VERSION 31

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

#include "common.h"
#include "logging.h"
#include "net.h"

#define TEST_DATA "hello world!\n"
#define DEFAULT_SEVER "slchiodo-pi.local"

/* Command line options */
static struct options {
    int show_help;
    int port;
    char *server;
} options;

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }

/* Command line option specification. We can add more here. If we're interested
 * in a string, specify --opt=%s .*/
static const struct fuse_opt option_spec[] = {
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    OPTION("--server=%s", server),
    OPTION("--port=%d", port),
    FUSE_OPT_END
};


/*
Func: get the attributes from the server for file contents
Param: path, stat struct, and file info
Return: integer
*/
static int netfs_getattr(
        const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

    LOG("getattr: %s\n", path);

    /* By default, we will return 0 from this function (success) */
    int res = 0;
    struct netfs_msg_header req_header = { 0 };
    req_header.msg_type = MSG_GETATTR;
    req_header.msg_len = strlen(path) + 1;

    /* Clear the stat buffer */
    memset(stbuf, 0, sizeof(struct stat));

    int server_fd;
    if ((server_fd = connect_to("localhost", options.port)) == -1) {
    	perror("connect to server");
    	return -1;
    }

    /* Send values to the server */
    write_len(server_fd, &req_header, sizeof(struct netfs_msg_header));
    write_len(server_fd, path, req_header.msg_len);
    read_len(server_fd, stbuf, sizeof(struct stat));

    return res;
}

/*
Func: get directory conents fromt the server
Param: path, void buf, filler to print to console, offset, file info and flags
Return: integer and displays contents on screen
*/
static int netfs_readdir (
        const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
        struct fuse_file_info *fi, enum fuse_readdir_flags flags) {

    LOG("readdir: %s\n", path);

    /* By default, will return 0 from this function (success) */
    int res = 0;
    struct netfs_msg_header req_header = { 0 };
    req_header.msg_type = MSG_READDIR;
    req_header.msg_len = strlen(path) + 1;

    /* Connect to the server */
    int server_fd;
    if ((server_fd = connect_to("localhost", options.port)) == -1) {
    	perror("connect to server");
    	return -1;
    }

    /* Send values to the server */
    write_len(server_fd, &req_header, sizeof(struct netfs_msg_header));
    write_len(server_fd, path, req_header.msg_len);

    uint16_t reply_len;
    char reply_path[MAX_PATH] = { 0 };
    do {
        read_len(server_fd, &reply_len, sizeof(uint16_t));
        read_len(server_fd, reply_path, reply_len);
        filler(buf, reply_path, NULL, 0, 0);
    } while (reply_len > 0);
    close(server_fd);
    return res;
}

/*
Func: checks if file can be openned by server
Param: path and file info
Return: integer 
*/
static int netfs_open (const char *path, struct fuse_file_info *fi) {

    LOG("open: %s\n", path);

    /* By default, we will return 0 from this function (success) */
    int res = 0;
    struct netfs_msg_header req_header = { 0 };
    req_header.msg_type = MSG_OPEN;
    req_header.msg_len = strlen(path) + 1;

    /* Connect to the server */
    int server_fd;
    if ((server_fd = connect_to("localhost", options.port)) == -1) {
    	perror("connect to server");
    	return -1;
    }

    /* Only supports opening the file in read-only mode */
    if ((fi->flags & O_ACCMODE) != O_RDONLY) { return -EACCES; }

    /* Send values to the server */
    write_len(server_fd, &req_header, sizeof(struct netfs_msg_header));
    write_len(server_fd, path, req_header.msg_len);

    uint16_t reply_bool;
    read_len(server_fd, &reply_bool, sizeof(uint16_t));
    if (reply_bool == 0) { 
    	LOG("%s\n", "Opened file."); 
    } else { 
    	LOG("%s\n", "Couldn't opened file."); 
    	res = 1; 
    }

    return res;
}

/*
Func: reads the contents of a file from server
Param: path, char buf, size, offset and file info
Return: integer and displays contents on screen
*/
static int netfs_read (
        const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi) {

    LOG("read: %s\n", path);

    struct netfs_msg_header req_header = { 0 };
    req_header.msg_type = MSG_READ;
    req_header.msg_len = strlen(path) + 1;

    /* Connect to the server */
    int server_fd;
    if ((server_fd = connect_to("localhost", options.port)) == -1) {
    	perror("connect to server");
    	return -1;
    }

    /* Send values to the server */
    write_len(server_fd, &req_header, sizeof(struct netfs_msg_header));
    write_len(server_fd, path, req_header.msg_len);
    write_len(server_fd, &offset, sizeof(off_t));
    write_len(server_fd, &size, sizeof(size_t));

    off_t out_size = 0;
    if ((out_size = read_len(server_fd, &out_size, sizeof(off_t))) < 0) {
    	perror("read from server\n");
    	return 1;
    }

    char reply_path[10000] = { 0 };
    read_len(server_fd, &reply_path, out_size); 
	LOG("READ %s\n", reply_path);

    memcpy(buf, &reply_path, out_size);
    return size;
}

/* This struct maps file system operations to our custom functions defined
 * above. */
static struct fuse_operations netfs_client_ops = {
    .getattr = netfs_getattr,
    .readdir = netfs_readdir,
    .open = netfs_open,
    .read = netfs_read,
};

static void show_help(char *argv[]) {
    printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
    printf("File-system specific options:\n"
    		"    --server=<n>        Server to connect to\n"
            "    --port=<n>          Port number to connect to\n"
            "                        (default: %d)"
            "\n", DEFAULT_PORT);
}

/*
Func: store values from command line in options struct
Param: cmd line numbre of values entered and array of values
Return: integer 
*/
void c_parse_args (int argc, char *argv[]) {
    int c;
    const char * short_opt = "s:p:f:";
	struct option long_opt[] = {
       {"port", required_argument, NULL, 'p'},
       {"server", required_argument, NULL, 's'},
       {NULL, 0,  NULL, 0}
  	};
    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
        	case -1:
        	case 0:
         		break;
        	case 'p':
         		options.port = atoi(optarg);
         		break;
        	case 's':
         		strcpy(options.server, optarg);
         		break;    
        };
    };
}



int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    /* Set up default options: */
    options.port = DEFAULT_PORT;
    options.server = malloc(sizeof(DEFAULT_SEVER));
    strcpy(options.server, DEFAULT_SEVER);
    c_parse_args(argc, argv);

    /* Parse options */
    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1) { 
    	perror("client cmd");
    	return 1; 
    }

    if (options.show_help) {
        show_help(argv);
        assert(fuse_opt_add_arg(&args, "--help") == 0);
        args.argv[0] = (char*) "";
    }
    free(options.server);
    return fuse_main(args.argc, args.argv, &netfs_client_ops, NULL);
}




