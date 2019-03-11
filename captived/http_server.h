#ifndef CAPTIVERC_HTTP_SERVER_HPP
#define CAPTIVERC_HTTP_SERVER_HPP

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <event2/event.h>
#include <event2/http.h>

#include "resource.h"

namespace captiverc {

class http_server {
   public:

    http_server(const int port);
    virtual ~http_server();
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
    static void send_json_response (struct evhttp_request *req, resource::resp_type response);

    static void not_found_cb (struct evhttp_request *req, void *arg);
    std::string get_control_address ();

   private:
    const int port_;
    std::unique_ptr<event_base, decltype(&event_base_free)> base_;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> httpd_;
    std::atomic<bool> running_;
};

inline void 
http_server::respond_not_allowed (struct evhttp_request *req, std::string err)
{
    evhttp_send_error(req, HTTP_BADMETHOD, err.c_str());
}



inline void 
http_server::respond_bad_request (struct evhttp_request *req, std::string err)
{
    evhttp_send_error(req, HTTP_BADREQUEST, err.c_str());
}


}    // namespace captiverc

#include "http_server.inl"

#endif    // CAPTIVERC_HTTP_SERVER_HPP
