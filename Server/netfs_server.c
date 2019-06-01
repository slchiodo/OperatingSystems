/**
 * netfs_server.h
 *
 * NetFS file server implementation.
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#include "common.h"
#include "logging.h"
#include "net.h"

#define MAX_BUF 1000
/*
Func: get the attributes for file contents
Param: file descriptor and msg header struct from client
Return: integer and file attributes are sent to the client
*/
int snetfs_getattr(int fd, struct netfs_msg_header req_header) {
    char path[MAX_PATH];
    read_len(fd, path, req_header.msg_len);
    LOG("getattr: %s\n", path);
    printf("-> getattr \n");

    char full_path[MAX_PATH] = { 0 };
    strcpy(full_path, ".");
    strcat(full_path, path);

    struct stat buf;
    stat(full_path, &buf); 

    // buf->st_mode = (mode_t) (~0666 & stbuf->st_mode);
    // if (strcmp(stbuf->st_uid, options.server) != 0) {
    // 	LOG("%ld",stbuf->st_uid);
    // 	strcpy(stbuf->st_uid, "root");
    // }

    write_len(fd, &buf, sizeof(struct stat));
    close(fd);
    return 0;
}

/*
Func: read the contents of a directory from client
Param: file descriptor and msg header struct from client
Return: integer and contents of the directory are sent to the client
*/
int snetfs_readdir (int fd, struct netfs_msg_header req_header) {
    char path[MAX_PATH];
    read_len(fd, path, req_header.msg_len);
    LOG("readdir: %s\n", path);

    char full_path[MAX_PATH] = { 0 };
    strcpy(full_path, ".");
    strcat(full_path, path);

    DIR *directory;
    if ((directory = opendir(full_path)) == NULL) {
        perror("readdir");
        close(fd);
        return 1;
    }

    // Sends results to the client
    uint16_t len;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        len = strlen(entry->d_name) + 1;
        write_len(fd, &len, sizeof(uint16_t));
        write_len(fd, entry->d_name, len);
    }

    // last directory entry
    len = 0;
    write_len(fd, &len, sizeof(uint16_t));
    closedir(directory);
    close(fd);
    return 0;   
} 

/*
Func: open a file from client
Param: file descriptor and msg header struct from client
Return: integer and a boolean value are sent to the client 
*/
int snetfs_open(int fd, struct netfs_msg_header req_header) {
    char path[MAX_PATH];
    read_len(fd, path, req_header.msg_len);
    LOG("open: %s\n", path);
    printf("-> open \n");

    char full_path[MAX_PATH] = { 0 };
    strcpy(full_path, ".");
    strcat(full_path, path);

    int file;
    uint16_t rv = 0;
    if ((file = open(full_path, O_RDONLY)) < 0) {
        rv = 1;
        write_len(fd, &rv, sizeof(uint16_t));
        close(fd);
        return 1;
    }

    write_len(fd, &rv, sizeof(uint16_t));
    close(file);
    close(fd);
    return 0;
}

/*
Func: read file from client
Param: file descriptor and msg header struct from client
Return:integer and the file contents are sent to the client
*/
int snetfs_read(int fd, struct netfs_msg_header req_header) {
    char path[MAX_PATH];
	off_t offset = 0;
	size_t size = 0;
    read_len(fd, path, req_header.msg_len);
    read_len(fd, &offset, sizeof(off_t));
    read_len(fd, &size, sizeof(size_t));

    LOG("read: %s\n", path);
    printf("-> read \n");

    char full_path[MAX_PATH] = { 0 };
    strcpy(full_path, ".");
    strcat(full_path, path);


    int file;
    if ((file = open(full_path, O_RDONLY)) < 0) {
    	perror("file read");
        close(fd);
        return 1;
    }

    struct stat buf;
    fstat(file, &buf); 
	

	off_t size_read = buf.st_size - offset;
	if (size_read <= 0) {
		perror("not long enough");
		return 1;
	}

	write_len(fd, &size_read, sizeof(off_t));
    off_t fileptr;
    if ((fileptr = lseek(file, offset, SEEK_SET)) == -1) {
    	perror("offset");
     	close(file);
    	close(fd);
    	return 1;	
    }

    char buffer[MAX_BUF];
    size_t r = 1;
    LOG("SIZE %lld\n", size_read);
	while (r > 0) {
		r = read(file, &buffer, size_read);
		LOG("%s\n", buffer);
	} 
	write_len(fd, &buffer, size_read);
    close(file);
    close(fd);


    /*

    int in_fd;
    if ((in_fd = open(full_path, O_RDONLY)) < 0) {
    	perror("file read");
        close(fd);
        return 1;
    }





	size_t b_size;
	while (left > 0) { 
		if (b_size < 0) {
			perror("sendfile");
			close(in_fd);
	    	close(fd);
	    	return 1;
		}
		b_size = sendfile(fd, in_fd, &offset, left);
		left -= b_size;	
	}
    close(in_fd);
    close(fd);
    */
    return 0;
}

/*
Func: Determine type of function when passed from client 
Param: file descriptor from client
Return: none 
*/
void handle_request (int fd) {
    struct netfs_msg_header req_header = { 0 };
    read_len(fd, &req_header, sizeof(struct netfs_msg_header));
    LOG("Handling request: [type %d; length %lld]\n", 
        req_header.msg_type, 
        req_header.msg_len );
    uint16_t type = req_header.msg_type;
    switch(type) {
        case MSG_READDIR  :
            snetfs_readdir(fd, req_header);
            break;
        case MSG_OPEN  :
            snetfs_open(fd, req_header);
            break;
        case MSG_READ :
            snetfs_read(fd, req_header);
            break;
        case MSG_GETATTR :
            snetfs_getattr(fd, req_header);
            break;            
        default : /* if type isn't valid */
            LOG("%s\n", "ERROR: Unknown request type.");
    }
}

/*
Func: Connect to serving directory
Param: path cmd line argument 
Return: none 
*/
void snetfs_dir (char *argv) {
    char tdir[MAX_PATH];
    strcpy(tdir, argv);
    char *dir = strtok (tdir, "/");
    int counter = 0;
    while (dir != NULL && counter < 1) {
        if (strcmp(dir, "home") == 0) {
            if (chdir(argv) != 0) { perror("cannot chdir"); }      	
        } else { perror("not in root"); }
        counter++;
    }
}

/*
Func: Change entered pot to integer
Param: port cmd line argument 
Return: port 
*/
int snetfs_port (char *argv) {
	int port = DEFAULT_PORT;
    if (argv != NULL) { port = atoi(argv); }
    return port;
}

int main(int argc, char *argv[]) {
    snetfs_dir(argv[1]);
    int port = snetfs_port(argv[2]);

    LOG("%s\n", "SERVER.");
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(socket_fd, 10) == -1) {
        perror("listen");
        return 1;
    }

    LOG("Listening on port %d\n", port);
    while ( true ) {
        struct sockaddr_storage client_addr = { 0 };
        socklen_t slen = sizeof(client_addr);
        int client_fd = accept(
                socket_fd,
                (struct sockaddr *) &client_addr,
                &slen);
        if (client_fd == -1) {
            perror("accept");
            return 1;
        }
        char remote_host[INET_ADDRSTRLEN];
        inet_ntop(
                client_addr.ss_family,
                (void *) &(((struct sockaddr_in *) &client_addr)->sin_addr),
                remote_host,
                sizeof(remote_host));
        LOG("Accepted connection from %s\n", remote_host);

	    pid_t child = fork();
	    if (child == 0) { handle_request(client_fd); } 
	    else if (child == -1) { perror("fork"); }
    }
    return 0; 
}
