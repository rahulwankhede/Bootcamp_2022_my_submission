#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>


void error(char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno;
	socklen_t clilen; //an unsigned opaque integral type of length of at least 32 bits

	char buffer[256];



	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if(argc < 2){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("ERROR opening socket");
	}

	/* fill in port number to listen on. IP address can be anything (INADDR_ANY)*/
	portno = atoi(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* bind socket to this port number on this machine (unlike clients, a server must bind to a well known address)*/
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	/* listen for incoming connection requests */
	listen(sockfd, 5); //(file descriptor, maximum length of queue of pending connections)
	clilen = sizeof(cli_addr);

	/* accept a new request and create a newsockfd */
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if(newsockfd < 0){
		error("ERROR on accept");
	}

	while(1){
		bzero(buffer, 256);
		n = read(newsockfd, buffer, 255);
		if(n < 0){
			error("ERROR reading from socket");
		}
		if(n == 1){
			break;
		}
		printf("%s", buffer);
	}

	close(newsockfd);
	close(sockfd);

	return 0;
}


