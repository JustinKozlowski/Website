#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define REQUESTS_MAX 50
#define MAX_MESSAGE_SIZE 3000

int main(int argc, char **argv){
	
	int server, client_socket;
	struct sockaddr_in server_addr, client_addr;
	long incoming;
	socklen_t addr_size;

	if( (server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "In server init");
		return 0;
	}

	//Setting variables in sockaddr_in see man 7 ip
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(2783139947);
	server_addr.sin_port = htons(PORT);

	//According to man bind(2)
	addr_size = sizeof(struct sockaddr_in);
	if(bind(server,(struct sockaddr *) &server_addr, addr_size) == -1){
		perror("In Bind");
		exit(EXIT_FAILURE);
	}
	if(listen(server, REQUESTS_MAX) == -1){
		perror("In Listen");
		exit(EXIT_FAILURE);
	}
	while(1){
		printf("Waiting for Connection...\n");
		if((client_socket = accept(server, (struct sockaddr *) &client_addr, &addr_size)) == -1){
			perror("In Accept");
			exit(EXIT_FAILURE);
		}
	       	printf("----------Connection Received----------\n");
		char buffer[MAX_MESSAGE_SIZE] = {'\0'};
		incoming = read(client_socket, buffer, MAX_MESSAGE_SIZE);
		printf("%s\n", buffer);
		//parse buffer and return appropriate information
		printf("--------------End Buffer--------------\n");
		close(client_socket);
	}
	return 0;
}
















