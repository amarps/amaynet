#include "Server.hxx"

#include <sys/socket.h>
#include <stdexcept>      // std::out_of_range
#include <iterator>
#include <iostream>
#include <map>
#include <exception>
#include <string.h>
#include <fstream>

std::vector<u_char> charToVector(const char* arg) {
  return std::vector<u_char>(arg, arg + strlen((char*)arg));
}

namespace AMAYNET
{
  const std::string HTTPServer::get_content_type(const std::string &path) {
    
    // will be defined once at the first call
    static const std::map<std::string, std::string> content_list {
      {".css", "text/css"},
      {".csv", "text/csv"},
      {".gif", "image/gif"},
      {".htm", "text/html"},
      {".html", "text/html"},
      {".ico", "image/x-icon"},
      {".jpeg", "image/jpeg"},
      {".jpg", "image/jpeg"},
      {".js", "application/javascript"},
      {".json", "applcation/json"},
      {".png", "image/png"},
      {".pdf", "application/pdf"},
      {".svg", "image/svg+xml"},
      {".txt", "text/plain"},
    };

    // return content_type of file_type. if file_type is not in content_list
    // return last content_type in content_list
    auto file_type = path.substr(path.find_last_of('.'));
    auto content_elm = content_list.find(file_type);

    if (content_elm == content_list.end()) {
      return std::prev(content_elm)->second;
    }
    return content_elm->second;
  }

  void HTTPServer::ServeResource(TCP *client, std::string &path) {

    if (path.compare("/") == 0)
      {
	path = "/index.html";
      }

    if (path.length() > 100) {
      SendHttpResponse(client, GetStatus(Status::BAD_REQUEST));
      return;
    }

    char full_path[128];
    sprintf(full_path, "public%s", path.c_str());

    std::ifstream ifs(full_path, std::ifstream::binary);
    ifs.seekg(0, ifs.end);
    int cl = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    

    if (!ifs.good()) {
      SendHttpResponse(client, GetStatus(Status::BAD_REQUEST));
      std::string errMsg = "Path does not exists ";
      errMsg.append(full_path);
      m_connection.Drop();
      return;
    }

    std::string ct = get_content_type(full_path);
    
    client->Send("HTTP/1.1 200 OK\r\n");  

    client->Send("Connection: close\r\n");

    client->Send(std::string("Content-Length: ")
		 .append(std::to_string(cl))
		 .append("\r\n"));

    client->Send(std::string("Content-Type: ")
		 .append(ct.c_str())
		 .append("\r\n"));

    client->Send("\r\n");

    char buffer[1024];
    while(!ifs.eof()) {
      ifs.read(buffer, 1024);
      client->Send(buffer, 1024);
    }

    ifs.close();
    m_connection.Drop();
  }

  void HTTPServer::SendHttpResponse(TCP *client, status_T status,
				const std::string &content) {
    std::stringstream res_stream;
    auto content_length = status.detail.length() + content.length();
    res_stream << "HTTP/1.1 " << status.code << " " << status.msg << "\r\n"
	       << "Connection: close" << "\r\n"
	       << "Content-Length: " << content_length
	       << "\r\n\r\n" << status.detail << content;
    client->Send(res_stream.str());
  }
  
  HTTPServer::HTTPServer(const std::string &port)
    : TCPListener()
      { }

  struct address_info {
    socklen_t address_length;
    struct sockaddr_storage address;
    char address_buffer[128];
  };

} // namespace AMAY::HTTP
