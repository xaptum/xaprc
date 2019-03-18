#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <event2/event.h>
#include <event2/http.h>

#include "rest/resource.hpp"

namespace captiverc {
namespace http {

class server {
   public:

    server(const int port, const std::string root_path);
    virtual ~server();
    void loop_dispatch();

    static void respond_not_allowed (struct evhttp_request *req, std::string err);
    static void respond_bad_request (struct evhttp_request *req, std::string err);


    bool is_get_request (struct evhttp_request *req){
        return (evhttp_request_get_command(req) == EVHTTP_REQ_GET);
    }
    bool is_put_request (struct evhttp_request *req){
        return (evhttp_request_get_command(req) == EVHTTP_REQ_PUT);
    }

    template<typename Resource> 
    int register_resource(Resource& rest_resource);

   private:
    static std::string get_payload(struct evhttp_request *req);
    static void send_json_response (struct evhttp_request *req, rest::resource::resp_type response);

    static void not_found_cb (struct evhttp_request *req, void *arg);
    std::string get_control_address ();

   private:
    const int port_;
    const std::string root_path_;
    std::unique_ptr<event_base, decltype(&event_base_free)> base_;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> httpd_;
    std::atomic<bool> running_;
};

inline void 
server::respond_not_allowed (struct evhttp_request *req, std::string err)
{
    evhttp_send_error(req, HTTP_BADMETHOD, err.c_str());
}



inline void 
server::respond_bad_request (struct evhttp_request *req, std::string err)
{
    evhttp_send_error(req, HTTP_BADREQUEST, err.c_str());
}


} // namespace http
} // namespace captiverc

#include "server.inl"
