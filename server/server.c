#include <sys/types.h>
#include <sys/socket.h>

#Define PORT 8080
#Define REQUESTS_MAX 50
#Define MAX_MESSAGE_SIZE

int main(int argc, char **argv){
	
	int server;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_size;

	if( server = socket(AF_INET, SOCKET_STREAM, 1) == -1) {
		fprintf(stderr, "In server init");
		return 0;
	}

	//Setting variables in sockaddr_in see man 7 ip
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	//According to man bind(2)
	addr_size = sizeof(struct sockaddr_in);
	if(bind(server,(struct sockaddr *) &address, addr_size) == -1){
		handle_error("In Bind");
	}
	if(listen(server, REQUESTS_MAX) == -1){
		handle_error("In Listen");
	}
	while(1){
		printf("Waiting for Connection...\n\n");
		if((client_socket = accept(server, (struct sockaddr *) &client_addr, addr_size) == -1)){
			handle_error("In Accept");
		}
	       	printf("----------Connection Received----------\n");
		char buffer[MAX_MESSAGE_SIZE] = '\0';
		incoming = (client_socket, buffer, MAX_MESSAGE_SIZE);
		printf("%s\n", buffer);
		//parse buffer and return appropriate information
		close(client_socket);
	}
	return 0;
}
