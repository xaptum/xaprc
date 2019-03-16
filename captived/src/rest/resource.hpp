#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <tuple>

#include "defines.hpp"
#include "json.hpp"
#include "http/status.hpp"

namespace captiverc {
namespace rest {

class resource{
  public:
    using resp_type = std::tuple<http::status, json::json_type>;
    using req_type = json::json_type;

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

  private:
    resp_type not_allowed(std::string method) {
        auto msg = method + " method not allowed for " + path_;
        return std::make_tuple(http::status::method_not_allowed,
                               json::string(msg));
    }

  private:
    std::string path_;      // URI path of resource

};

} // namespace rest
} // namespace captiverc

#endif
