#pragma once

#include <string>
#include <tuple>

#include "defines.hpp"
#include "json.hpp"
#include "http/status.hpp"

namespace captived {
namespace rest {

class resource{
  public:
    using req_body_type = json::json_type;
    using req_type = req_body_type;

    using resp_body_type = json::json_type;
    using resp_type = std::tuple<http::status, resp_body_type>;

  public:
    resource(std::string path):
                path_(path)
        {}

    virtual ~resource() = default;

    std::string get_path() {return path_;}

    virtual resp_type get(req_type body) {
        return not_allowed("GET");
    }

    virtual resp_type put(req_type body) {
        return not_allowed("PUT");
    }

    virtual resp_type post(req_type body) {
        return not_allowed("POST");
    }

  protected:
    virtual resp_type ok(resp_body_type body) {
        return {http::status::ok, body};
    }

    virtual resp_type bad_request(resp_body_type body) {
        return {http::status::bad_request, body};
    }

    virtual resp_type method_not_allowed(resp_body_type body) {
        return {http::status::method_not_allowed, body};
    }

    virtual resp_type internal_server_error(resp_body_type body) {
        return {http::status::internal_server_error, body};
    }

  private:
    resp_type not_allowed(std::string method) {
        auto msg = method + " method not allowed for " + path_;
        return method_not_allowed(json::string(msg));
    }

  private:
    std::string path_;      // URI path of resource

};

} // namespace rest
} // namespace captived
