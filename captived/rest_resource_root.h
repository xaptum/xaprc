#ifndef REST_RESOURCE_ROOT_H
#define REST_RESOURCE_ROOT_H

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "http_server.h"
#include "rest_resource_generic.h"

namespace captiverc {

class rest_resource_root : public rest_resource_generic {
  public:
    rest_resource_root(std::string uri, 
                          http_server* server);

    virtual void callback(struct evhttp_request *req, void *arg);

  private:
    std::string get_file_contents(std::string filename);
    std::string get_status();


};

}   // namespace captiverc


#endif
