#ifndef REST_RESOURCE_ROUTER_MODE_H
#define REST_RESOURCE_ROUTER_MODE_H

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "http_server.h"
#include "rest_resource_generic.h"

namespace captiverc {

class rest_resource_router_mode : public rest_resource_generic {
  public:
    rest_resource_router_mode(std::string uri, 
                          http_server* server,
                          std::string filename);

    virtual void callback(struct evhttp_request *req, void *arg);

  private:
    int set_file_contents(std::string payload);
    std::string get_file_contents();


  private:
    std::string filename_;


};

}   // namespace captiverc


#endif
