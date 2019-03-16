#ifndef CAPTIVERC_HTTP_RESPONSE_CODES_HPP
#define CAPTIVERC_HTTP_RESPONSE_CODES_HPP

namespace captiverc {
namespace http {

  /**
   * The HTTP status codes that may be returned by the captived
   * server.
   */
  enum class status : unsigned
  {
   ok      = 200,

   bad_request        = 400,
   not_found          = 404,
   method_not_allowed = 405,

   internal_server_error = 500
  };

} // namespace http
} // namespace captiverc

#endif
