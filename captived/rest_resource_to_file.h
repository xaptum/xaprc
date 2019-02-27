#ifndef REST_RESOURCE_TO_FILE_H
#define REST_RESOURCE_TO_FILE_H

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "http_server.h"
#include "rest_resource_generic.h"

namespace captiverc {

class rest_resource_to_file : public rest_resource_generic {
  public:
    rest_resource_to_file(std::string uri, 
                          http_server* server,
                          std::string filename,
                          bool allow_put);

    virtual void callback(struct evhttp_request *req, void *arg);

  private:
    int set_file_contents(std::string payload);
    std::string get_file_contents();


  private:
    std::string filename_;
    bool allow_put_;


};

}   // namespace captiverc


#endif