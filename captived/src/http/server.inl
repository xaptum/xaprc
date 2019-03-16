#include <algorithm>
#include <cstring>
#include <event2/buffer.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "defines.hpp"
#include "json.hpp"

namespace captiverc {
namespace http {

////////////////////////////////////////////////////////////////////////////////
/// register_resource
/// Register a resource object from which an http callback will be made.
/// resource-based approach (class centric)
////////////////////////////////////////////////////////////////////////////////
template<typename Resource>
int
server::register_resource(Resource& rest_resource)
{
  auto path = rest_resource.get_path();

  auto op = [](struct evhttp_request* req, void* arg) {
              auto action = evhttp_request_get_command(req);
              auto headers = evhttp_request_get_input_headers(req);

              Resource* resourcep = static_cast<Resource*>(arg);

              // Parse the body
              json::error_type error;
              auto body = json::loads(server::get_payload(req), &error);
              if (!body) {
                std::stringstream msg;
                msg << "JSON parsing error on line " << error.line << ": "
                    << error.text << std::endl;
                server::respond_bad_request(req, msg.str());
                return;
              }

              typename Resource::resp_type resp;
              // create get/put in the parent class - return an error
              // child should override this if valid for that child.
              if (action == EVHTTP_REQ_GET ) {
                // have resource.get return a tuple {return code, string}
                resp = resourcep->get(body);
              } else if (action == EVHTTP_REQ_PUT) {
                const char* content = evhttp_find_header(headers, "Content-Type");
                if (content
                    && (std::strncmp(content, CONTENT_TYPE_JSON, std::strlen(CONTENT_TYPE_JSON)) != 0 )
                   ){
                  server::respond_bad_request(req,
                                                   "Error - content type must be JSON.");
                  return;
                }
                resp = resourcep->put(body);
              } else if (action == EVHTTP_REQ_POST) {
                const char* content = evhttp_find_header(headers, "Content-Type");
                if (content 
                    && (strncmp(content, CONTENT_TYPE_JSON, strlen(CONTENT_TYPE_JSON)) != 0 )
                   ){
                  server::respond_bad_request(req, 
                                        "Error - content type must be JSON.");
                  return;
                }
                resp = resourcep->post(body);
              } else {
                server::respond_not_allowed(req,
                                                 "Error - Request type not allowed.");
                  return;
              }

              server::send_json_response(req, resp);
  };

  evhttp_set_cb(httpd_.get(), path.c_str(), op, &rest_resource);

  return 0;
}

}    // namespace captiverc
