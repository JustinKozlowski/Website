#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define PORT 80
#define REQUESTS_MAX 50
#define MAX_MESSAGE_SIZE 3000
#define IP_ADDRESS 2783139947

#define GET "GET"
#define POST "POST"
#define HEAD "HEAD"
#define TRACE "TRACE"
#define OPTIONS "OPTIONS"
#define CONNECT "CONNECT"

#define READ "r"

#define SOURCE "../src"
#define SOURCE_SIZE 6
#define SOURCE_DEFAULT "/index.html"
#define SOURCE_DEFAULT_SIZE 11
#define GET_RESP_HEADER "HTTP/1.1 200 OK\nDate: %a, %d %b %G %T %Z\nServer: www.justinkozlowski.me\nContent-Type: text/html;charset=UTF-8\nContent-Length: "
#define GET_RESP_HEAD_SIZE 150 //Accounts for characters added for the date repacements +2 for the newline characters after the content-Length
#define NOT_FOUND_HEADER "HTTP/1.1 404 Not Found\nDate: %a, %d %b %G %T %Z\nServer: www.justinkozlowski.me\n"
#define NOT_FOUND_HEAD_SIZE 100

//void parse(char *buffer);
int request(char *buffer, int client_socket);
int get_request(char *buffer, int client_socket);
FILE *get_file_src(char *filename, int size);
int post_request(char *buffer, int client_socket);
char *server_time(char *string, int size);

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
	server_addr.sin_addr.s_addr = htonl(IP_ADDRESS);
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
		if(!incoming){
			printf("Empty Message\n");
			close(client_socket);
		}
		else{
			printf("%s\n", buffer);
			printf("--------------End Buffer--------------\n");
			//parse buffer and return appropriate information
			request(buffer, client_socket);
			close(client_socket);
		}
	}
	return 0;
}

//void parse(char *buffer){
//	char *request = buffer;
//}

int request(char *buffer, int client_socket){
	char *request = buffer;
	printf("Checking Request Type\n");
	if(!strncmp(request, GET, 3)){
		buffer = (buffer + 4);
		return get_request(buffer,client_socket);
	}
	if(!strncmp(request, POST, 4)){
		buffer = (buffer + 4);
		return post_request(buffer,client_socket);
	}
//	if(strncmp(request, PUT, 3)){
//		buffer = (buffer + 3);
//		return put_request(buffer);
//	}
//	if(strncmp(request, TRACE, 5)){
//		buffer = (buffer + 5);
//		return trace_request(buffer);
//	}
//	if(strncmp(request, OPTIONS, 7)){
//		buffer = (buffer + 7);
//		return options_request(buffer);
//	}
//	if(strncmp(request, CONNECT, 7)){
//		buffer = (buffer + 7);
//		return connect_request(buffer);
//	}
	printf("Error 404, Request not found\n");
	return 404; //Request not found

}

int get_request(char *buffer, int client_socket){
	printf("Serving Request\n");
	//Parses for file name
	int i = 0;
	char buffer_char = buffer[i];
	while(buffer_char != ' '){
		buffer_char = buffer[i++];
	}
	i--;
	char file[i+1];
	strncpy(file, buffer, i);
	file[i] = '\0';
	printf("Searching for file(size i=%i): %s.\n", i, file);
	FILE *src_file = get_file_src(file, i);
	//reads file to buffer
	printf("Opened File: %p\n", src_file);

	//File not found section
	if(src_file == NULL){
		printf("File Not Found\n");
		char *output = calloc((NOT_FOUND_HEAD_SIZE), sizeof(char));
		char *request_time = server_time(NOT_FOUND_HEADER, NOT_FOUND_HEAD_SIZE);
		printf("%s", request_time);
		strcat(output, request_time);
		free(request_time);
		printf("Sending:\n\n%s\n", output);
	
		//Send Response over socket
		write(client_socket, output, strlen(output));
		return 404;
	}

	fseek(src_file, 0, SEEK_END);
	long src_size = ftell(src_file);
	void *body = calloc(src_size, 1);
	rewind(src_file);
	if(fread(body, sizeof(char), src_size, src_file) < src_size){
		perror("Did not read src properly -- GET\n");
	}
	printf("Read File\n%s\n", (char*)body);

	//This section builds the GET response and catonates it together
	char *content_len = calloc(16, sizeof(char));
	sprintf(content_len, "%ld", src_size);
	int cont_len_size = strlen(content_len);
	char *output = calloc((cont_len_size + GET_RESP_HEAD_SIZE + src_size), sizeof(char));
	char *request_time = server_time(GET_RESP_HEADER, GET_RESP_HEAD_SIZE);
	strcat(output, request_time);
	free(request_time);
	strcat(output, content_len);
	free(content_len);
	strcat(output, "\n");
	strcat(output, "\n");
	strcat(output, body);
	free(body);
	printf("Sending:\n\n%s\n", output);

	//Send Response over socket
	write(client_socket, output, strlen(output));
	free(output);
	return 200;
}

//Not yet implemented
int post_request(char *buffer, int client_socket){
	return 400;
}

//Return file descriptor of the file
FILE *get_file_src(char *filename, int size){
	printf("filename: '%s'\n", filename);
	if(strcmp(filename, "/\0") == 0){
		size = SOURCE_DEFAULT_SIZE;
		char file_loc[SOURCE_SIZE + size];
		strcpy(file_loc, SOURCE);
       		strcat(file_loc, SOURCE_DEFAULT);
		printf("Opening file %s\n", file_loc);
		return fopen(file_loc, READ);
	}
	char file_loc[SOURCE_SIZE + size];
	strcpy(file_loc, SOURCE);
       	strcat(file_loc, filename);
	printf("Opening file %s\n", file_loc);
	return fopen(file_loc, READ);
}

//Creates server time in header string
char *server_time(char *string, int size){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char *request_time = calloc(size, sizeof(char));
	strftime(request_time,size,string,timeinfo);
	return request_time;
}





