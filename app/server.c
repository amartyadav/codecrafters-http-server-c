#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	// printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	//
	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(4221),
									 .sin_addr = { htonl(INADDR_ANY) },
									};

	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);

	int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
	printf("Client connected\n");

	// buffer for the incoming request
	char reqBuff[1024];
	recv(fd, reqBuff, sizeof(reqBuff), 0);

	printf(reqBuff);

	char *path = NULL;
	char *first_space = strchr(reqBuff, ' ');

	if (first_space) {
	   char *second_space = strchr(first_space + 1, ' ');
		if (second_space) {
		  int path_length = second_space - (first_space + 1);
			path = malloc(path_length + 1);
			if (path) {
			    strncpy(path, first_space + 1, path_length);
				path[path_length] = '\0';
				printf("Extracted path: %s\n", path);
			}
		}
	}



	if (strcmp(path, "/") == 0) {
	   send(fd, "HTTP/1.1 200 OK\r\n\r\n", strlen("HTTP/1.1 200 OK\r\n\r\n"), MSG_EOR);
	}
	else {
	   send(fd, "HTTP/1.1 404 Not Found\r\n\r\n", strlen("HTTP/1.1 404 Not Found\r\n\r\n"), MSG_EOR);
	}

	close(server_fd);
	if (path != NULL) {
        free(path);
        path = NULL;
	}

	return 0;
}
