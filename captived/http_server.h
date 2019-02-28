#ifndef CAPTIVERC_HTTP_SERVER_HPP
#define CAPTIVERC_HTTP_SERVER_HPP

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include <event2/event.h>
#include <event2/http.h>

namespace captiverc {

class http_server {
   public:
    http_server(const int port);
    virtual ~http_server();
    void loop_dispatch();

    static std::string get_file_contents(std::string filename);
    static void serve_config_file_as_json(struct evhttp_request* req,
                                          void* arg,
                                          std::string filename);
    static void get_put_config_json(struct evhttp_request* req,
                                          void* arg,
                                          std::string filename,
                                          bool allowPut);
    void respond_not_allowed (struct evhttp_request *req, std::string err);
    void respond_bad_request (struct evhttp_request *req, std::string err);
    std::string get_payload(struct evhttp_request *req);
    void send_json_response (struct evhttp_request *req, const char* response);
    int register_callback (std::string uri,
                           void (*cb)(struct evhttp_request *, void *), 
                           void* arg);



    bool is_get_request (struct evhttp_request *req){
        return (evhttp_request_get_command(req) == EVHTTP_REQ_GET);
    }
    bool is_put_request (struct evhttp_request *req){
        return (evhttp_request_get_command(req) == EVHTTP_REQ_PUT);
    }



   private:
    void test_cb(struct evhttp_request* req, void* arg);
    static void full_status_cb(struct evhttp_request* req, void* arg);
    static void dispatch_stop_cb(evutil_socket_t fd, short what, void* arg);
    static void not_found_cb (struct evhttp_request *req, void *arg);
    std::string get_control_address ();

    void gen_request_cb(struct evhttp_request* req);

    unsigned long max_metric_name_length();
    void write_header(std::ostream& out, int name_width);
    void write_version(std::ostream& out);

    static std::string get_status();

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

#endif    // CAPTIVERC_HTTP_SERVER_HPP
