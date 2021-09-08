#pragma once

#ifndef _AMAY_HTTP_STATUS_H
#define _AMAY_HTTP_STATUS_H

#include <string>
#include <vector>

namespace AMAYNET
{
  /**
   * 
   * message and detail from https://httpstatuses.com
   */
  struct status_T {
    int code;
    std::string msg;
    std::string detail;
  
    status_T(int _code, const std::string &_msg, const std::string &_detail)
      :code(_code),
       msg(_msg),
       detail(_detail) {}
  };

  enum Status {
    // Success status
    OK,
    CREATED,
    ACCEPTED,

    // Client Error
    BAD_REQUEST,
    UNAUTHORIZED,
    FORBIDDEN,
    NOT_FOUND,
    REQUEST_TIMEOUT,

    // Server Error
    NOT_IMPLEMENTED,
    HTTP_VERSION_NOT_SUPPORTED,
  };

  /**
   *
   */
  inline const status_T GetStatus(Status _status) {
    static const std::vector<status_T> v_status = {
      // Success status
      status_T(200, "OK", "The request has succeeded."),
      status_T(201, "Created",
               "The request has been fulfilled and has resulted in one or more "
               "new resources being created."),
      status_T(202, "Accepted",
               "The request has been accepted for processing, but the "
               "processing has not been completed. The request might or might "
               "not eventually be acted upon, as it might be disallowed when "
               "processing actually takes place."),

      // Client Error
      status_T(400, "Bad Request",
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
      status_T(505, "HTTP Version Not Supported",
               "The server does not support, or refuses to support, the major "
               "version of HTTP that was used in the request message.")};

    return v_status[_status];
  }

} // namespace AMAY::HTTP

#endif // AMAY_HTTP_STATUS_H
