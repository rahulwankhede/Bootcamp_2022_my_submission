#include "http_server.h"
#include <vector>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <chrono>
//#include <ctime>
//#include <iostream>

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
