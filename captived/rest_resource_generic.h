#ifndef REST_RESOURCE_GENERIC_H
#define REST_RESOURCE_GENERIC_H

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "http_server.h"

namespace captiverc {

class rest_resource_generic {
  public:
    rest_resource_generic(std::string uri, http_server* server);
    std::string serialize_json();
    std::string parse_json(std::string);
    std::string get_uri() { return uri_;};
    virtual void callback(struct evhttp_request *req, void *arg) = 0;

  protected:
    std::string uri_;
    http_server* serverp_;
    std::string req_type_string(enum evhttp_cmd_type req_type);
    void respond_op_not_allowed(struct evhttp_request* req);
    
};

} //namespace captiverc

#endif