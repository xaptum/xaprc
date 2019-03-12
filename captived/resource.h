#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <tuple>

#include <event2/http.h>        // using only for HTTP return codes

#include "defines.h"
#include "json.h"

namespace captiverc {


class resource{
  public:
    using resp_type = std::tuple<int, json::json_type>;
    using req_type = json::json_type;

  public:
    resource(std::string path):
                path_(path)
        {}

    virtual ~resource() = default;

    std::string get_path() {return path_;}

    // default handler will return an error - each subclass
    // must replace this if the operation is valid.
    virtual resp_type get(req_type body) {
        auto msg = "GET operation not allowed for " + path_;
        return std::make_tuple(HTTP_BADMETHOD, json::string(msg));
    }

    virtual resp_type put(req_type body) {
        auto msg = "PUT operation not allowed for " + path_;
        return std::make_tuple(HTTP_BADMETHOD, json::string(msg));
    }

    virtual resp_type post(req_type body) {
        auto msg = "POST operation not allowed for " + path_;
        return std::make_tuple(HTTP_BADMETHOD, json::string(msg));
    }

  private:
    std::string path_;      // URI path of resource

};

}   //namespace captiverc

#endif
