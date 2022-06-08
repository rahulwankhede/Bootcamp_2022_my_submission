#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> // for bzero()
#include <unistd.h> // for read() and write()

#include <sys/socket.h> // works without including
#include <sys/types.h>	// these two files as well

#include <netinet/in.h> // for struct sockaddr_in
#include <netdb.h> // for gethostbyname()


/* helper function for printing error messages */
void error(char *msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]){
	int sockfd, portno, n;

	/* 
	 * #include <netinet/in.h>
	 *
	 * struct sockaddr_in {
	 * 	   short            sin_family;   // e.g. AF_INET
	 *	   unsigned short   sin_port;     // e.g. htons(3490)
	 *	   struct in_addr   sin_addr;     // see struct in_addr, below
	 *	   char             sin_zero[8];  // zero this if you want to
	 * };
	 *
	 * struct in_addr{
	 * 	   unsigned long s_addr;  //load with inet_aton()
	 * };
	 *
	 */
	struct sockaddr_in serv_addr;
	
	/*
	 * struct hostent {
	 * 	   char *h_name;       // official name of host
	 * 	   char **h_aliases;   // alias list
	 * 	   int h_addrtype;     // host address type
	 * 	   int h_length;       // length of address
	 * 	   char **h_addr_list; // list of addresses
	 * };
	 *
	 */
	struct hostent *server;


	char buffer[256]; // to hold message
	if(argc < 3){
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	/* create socket, get sockfd handle */
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}

	/* take server address from command line (into struct hostent * data structure pointer named "server")*/
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	/* fill server address into sockaddr_in data structure */
	bzero( (char *) &serv_addr, sizeof(serv_addr) ); //set arg0 to arg1 bytes of zeros
	serv_addr.sin_family = AF_INET; //AF_INET for IPv4 Internet Protocol, AF_UNIX or AF_LOCAL for local communication
	serv_addr.sin_port = htons(portno); //uint16_t htons(uint16_t hostshort); converts the unsigned short integer hostshort from host byte order to network byte order.
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length); // src, dest, bytes
	/* define this on top if you get "struct "hostent" has no field "h_addr" error
	 *  #define h_addr h_addr_list[0] // for backward compatibility
	 */

	/* connect to server */
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR connecting");
	}
	/* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) connects the socket referred to by the file descriptor sockfd 
	 * to the address specified by addr; addrlen refers to the size of addr
	 */

	/* My loop part starts here */
	while(1){
		bzero(buffer, 256);
		fgets(buffer, 255, stdin); // (dest, bytes, src)
		n = write(sockfd, buffer, strlen(buffer));
		if(n < 0){
			error("ERROR writing to socket\n");
		}
		if(n == 1){
			break;
		}
	}

	/* close socket */
	close(sockfd);

	return 0;
}
