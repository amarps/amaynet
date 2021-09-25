#include "HTTP/Server.hxx"

#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string.h>
#include <sys/socket.h>

namespace AMAYNET
{
  std::string HTTPServer::GetContentType(const std::string &path) {
    
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

  HTTPRequest HTTPServer::GetRequest() {
    auto recv_obj = CurrentConnection()->Recv();
    std::string msg(recv_obj.begin(), recv_obj.end());
    // check if request msg contain \r\n\r\n that indicate end of request
    if (msg.find("\r\n\r\n") == std::string::npos) {
      return HTTPRequest(std::string(), HTTPRequest::INVALID);
    }
      
    auto requestMethod = HTTPRequest::INVALID;
    auto path = std::string();

    const int num_method_element = 2;
    std::string str_req_methods[num_method_element] {
      "GET /",
	"POST /"
	};

    for (int mthd_i=0; mthd_i < num_method_element; mthd_i++) {
      if (msg
	  .compare(0, str_req_methods[mthd_i].length(),
		   str_req_methods[mthd_i]) == 0) {
	path = msg.substr(str_req_methods[mthd_i].length() - 1);
	requestMethod = (HTTPRequest::Method)(mthd_i + 1);
	break;
      }
    }

    auto end_path_pos = path.find(' ');
    if (end_path_pos == std::string::npos) { // invalid path. space not found
      return HTTPRequest(std::string(), HTTPRequest::INVALID);
    }

    path = path.substr(0, end_path_pos);
    return HTTPRequest(path, requestMethod);
  };

  void HTTPServer::ServeResource(std::string &path, int chunk_size) {

    if (path == "/")
      {
	path = "/index.html";
      }

    constexpr int max_path_len = 100;

    if (path.length() > max_path_len) {
      SendResponse(GetStatus(Status::BAD_REQUEST));
      return;
    }

    std::string full_path = GetHostDir();
    full_path.append(path);

    std::ifstream ifs(full_path, std::ifstream::binary);
    ifs.seekg(0, std::ifstream::end);
    int cl = ifs.tellg();
    ifs.seekg(0, std::ifstream::beg);
    

    if (!ifs.good()) {
      SendResponse(GetStatus(Status::BAD_REQUEST));
      std::string errMsg = "Path does not exists ";
      errMsg.append(full_path);
      DropConnection();
      return;
    }

    std::string ct = GetContentType(full_path);

    auto *client = CurrentConnection();
    
    client->Send("HTTP/1.1 200 OK\r\n");  

    client->Send("Connection: close\r\n");

    client->Send(std::string("Content-Length: ")
		 .append(std::to_string(cl))
		 .append("\r\n"));

    client->Send(std::string("Content-Type: ")
		 .append(ct)
		 .append("\r\n"));

    client->Send("\r\n");

    char buffer[chunk_size];
    while(!ifs.eof()) {
      ifs.read(buffer, chunk_size);
      client->Send(buffer, chunk_size);
    }

    ifs.close();
    DropConnection();
  }

  void HTTPServer::SendResponse(const status_T& status,
				const std::string &content) {
    std::stringstream res_stream;
    auto content_length = status.detail.length() + content.length();
    res_stream << "HTTP/1.1 " << status.code << " " << status.msg << "\r\n"
	       << "Connection: close" << "\r\n"
	       << "Content-Length: " << content_length
	       << "\r\n\r\n" << status.detail << content;
    
    CurrentConnection()->Send(res_stream.str());
  }
  
  HTTPServer::HTTPServer(const std::string &port)
    :TCPListener(port)
  { }

  status_T HTTPServer::GetStatus(Status _status) {
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

} // namespace AMAYNET
