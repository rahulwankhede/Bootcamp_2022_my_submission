#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>

#include "http_server.h"
#include <vector>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <chrono>
//#include <ctime>
//#include <iostream>


#define NUM_THREADS 5


vector<string> split(const string &s, char delim) {
    vector<string> elems;

    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        if(!item.empty())
            elems.push_back(item);
    }

    return elems;
}

HTTP_Request::HTTP_Request(string request)
{
    vector<string> lines = split(request, '\n');
    vector<string> first_line = split(lines[0], ' ');

    /*
     TODO : extract the request method, URI and HTTP version from first_line here
    */
	this->method = first_line[0];
	this->url = first_line[1];
	this->HTTP_version = first_line[2].substr(5);

    this->HTTP_version = "1.0";  //We'll be using 1.0 irrespective of the request

    if(this->method != "GET")
    {
        cerr<<"Method '"<<this->method<<"' not supported"<<endl;
        exit(1);
    }
}


HTTP_Response* handle_request(string req)
{
    HTTP_Request *request = new HTTP_Request(req);

    HTTP_Response *response = new HTTP_Response();

    string url = string("html_files/") + request->url;

    response->HTTP_version = "1.0";

    struct stat sb;
    if(stat(url.c_str(), &sb) == 0)  // requested path exists
    {
        response->status_code = "200";
        response->status_text = "OK";

        response->content_type = "text/html";
        string body;

        if(S_ISDIR(sb.st_mode))
        {
            /*
            In this case, requested path is a directory.
            TODO : find the index.html file in that directory (modify the url accordingly)
            */
			url = url + string("index.html");
        }

        /*
        TODO : open the file and read its contents
        */
		std::ifstream t(url);
		std::stringstream buffer;
		buffer << t.rdbuf();
		body = buffer.str(); // fill the body string here


        /*
        TODO : set the remaining fields of response appropriately
        */
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);

		string date = string(std::ctime(&now_time));

		response->date = string("Date: ") + date.substr(0, 3) + string(", ") + date.substr(8, 3) + date.substr(4, 4) + date.substr(20, 4) + date.substr(10, 10) + string("GMT") ; // fill this from current time in the appropriate format
		response->body = body;
		response->content_length = body.length();
    }

    else
    {
        response->status_code = "404";
        response->status_text = "Not Found";

        /*
        TODO : set the remaining fields of response appropriately
        */
		response->content_type = "text/html";

		response->body = "404 Error not found";

		response->content_length = response->body.length();

    }

    delete request;

    return response;
}


string HTTP_Response::get_string()
{
    /*
    TODO : implement this function
    You can use sprintf to print to a string
    */
	return string("HTTP/") + this->HTTP_version + string(" ") + this->status_code + string(" ") + this->status_text + string("\nDate: ") + 
		this->date + string("\nContent-Type: ") + this->content_type + string("\nContent-Length: ") + std::to_string((int)(this->body).length()) + string("\n\n") + 
		this->body;
}





void error(char *msg){
	perror(msg);
	exit(1);
}

void *echo_fn(void * newsockfd_ptr){
	char buffer[256];
	int n, newsockfd;
//	while(1){
		bzero(buffer, 256);
		newsockfd = *(int *)newsockfd_ptr;
		n = read(newsockfd, buffer, 255);
		if(n < 0){
			error((char *)"ERROR reading from socket");
		}
		//printf("%s", buffer);

//		printf("buffer = %s\n", buffer);
		
		string request_str = buffer;

//		printf("request_str = %s", request_str.c_str());

		printf("Here 1\n");

		HTTP_Response *my_response = handle_request(request_str);

		printf("Here 2\n");

		string output_str = my_response->get_string();

		n = write(newsockfd, output_str.c_str(), 20480);

		delete my_response;
//	}
	close(newsockfd);
	
	return NULL;
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
		error((char *)"ERROR opening socket");
	}

	/* fill in port number to listen on. IP address can be anything (INADDR_ANY)*/
	portno = atoi(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* bind socket to this port number on this machine (unlike clients, a server must bind to a well known address)*/
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error((char *)"ERROR on binding");
	}

	/* listen for incoming connection requests */
	listen(sockfd, 2); //(file descriptor, maximum length of queue of pending connections)
	clilen = sizeof(cli_addr);

	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; i++){
		/* accept a new request and create a newsockfd */
		thread_args[i] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0){
			error((char *)"ERROR on accept");
		}
		else{
			printf("Creating thread %d\n", i);
			pthread_create(&threads[i], NULL, echo_fn, &thread_args[i]);
		}
	}

	close(sockfd);

	return 0;
}


