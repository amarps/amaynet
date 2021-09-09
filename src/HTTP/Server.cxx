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
  const std::string HTTPServer::GetContentType(const std::string &path) {
    
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

  void HTTPServer::ServeResource(std::string &path) {

    if (path.compare("/") == 0)
      {
	path = "/index.html";
      }

    if (path.length() > 100) {
      SendResponse(GetStatus(Status::BAD_REQUEST));
      return;
    }

    char full_path[128];
    sprintf(full_path, "public%s", path.c_str());

    std::ifstream ifs(full_path, std::ifstream::binary);
    ifs.seekg(0, ifs.end);
    int cl = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    

    if (!ifs.good()) {
      SendResponse(GetStatus(Status::BAD_REQUEST));
      std::string errMsg = "Path does not exists ";
      errMsg.append(full_path);
      m_connection.Drop();
      return;
    }

    std::string ct = GetContentType(full_path);

    auto client = m_connection.data();
    
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

  void HTTPServer::SendResponse(status_T status,
				const std::string &content) {
    std::stringstream res_stream;
    auto content_length = status.detail.length() + content.length();
    res_stream << "HTTP/1.1 " << status.code << " " << status.msg << "\r\n"
	       << "Connection: close" << "\r\n"
	       << "Content-Length: " << content_length
	       << "\r\n\r\n" << status.detail << content;
    
    m_connection.data()->Send(res_stream.str());
  }
  
  HTTPServer::HTTPServer(const std::string &port)
    : TCPListener()
  { }

  struct address_info {
    socklen_t address_length;
    struct sockaddr_storage address;
    char address_buffer[128];
  };

  const HTTPServer::status_T HTTPServer::GetStatus(Status _status) {
    static const std::vector<status_T> v_status = {
        // Success status
        status_T(200, "OK", "The request has succeeded."),
        status_T(
            201, "Created",
            "The request has been fulfilled and has resulted in one or more "
            "new resources being created."),
        status_T(
            202, "Accepted",
            "The request has been accepted for processing, but the "
            "processing has not been completed. The request might or might "
            "not eventually be acted upon, as it might be disallowed when "
            "processing actually takes place."),

        // Client Error
        status_T(
            400, "Bad Request",
            "The server cannot or will not process the request due to "
            "something that is perceived to be a client error (e.g., "
            "malformed request syntax, invalid request message framing, or "
            "deceptive request routing)."),
        status_T(401, "Unauthorized",
                 "The request has not been applied because it lacks valid "
                 "authentication credentials for the target resource."),
        status_T(
            403, "Forbidden",
            "The server understood the request but refuses to authorize it."),
        status_T(
            404, "Not Found",
            "The origin server did not find a current representation for the "
            "target resource or is not willing to disclose that one exists."),
        status_T(408, "Request Timeout",
                 "The server did not receive a complete request message within "
                 "the time that it was prepared to wait."),

        // Server Error
        status_T(501, "Not Implemented",
                 "The server does not support the functionality required to "
                 "fulfill the request."),
        status_T(
            505, "HTTP Version Not Supported",
            "The server does not support, or refuses to support, the major "
            "version of HTTP that was used in the request message.")};

    return v_status[_status];
  }

} // namespace AMAY::HTTP
