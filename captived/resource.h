#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <tuple>

#include <event2/http.h>        // using only for HTTP return codes
#include "defines.h"

namespace captiverc {


class resource{
  public:
    using resp_type = std::tuple<int, std::string>;
    using req_type = std::string;

  public:
    resource(std::string path):
                path_(path)
        {}

    virtual ~resource() = default;

    std::string get_path() {return path_;}

    // default GET / PUT handler will return an error - each subclass 
    // must replace this if the operation is valid.
    virtual resp_type get(req_type body) {
        return std::make_tuple(HTTP_BADMETHOD, 
                                "GET operation not allowed for " + path_);}
    virtual resp_type put(req_type body){
        return std::make_tuple(HTTP_BADMETHOD, 
                                "PUT operation not allowed for " + path_);}

  private:
    std::string path_;      // URI path of resource

};

}   //namespace captiverc

#endif
