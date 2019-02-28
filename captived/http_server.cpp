#include <algorithm>
#include <event2/buffer.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <jansson.h>
#include <locale>
#include <sstream>
#include <unistd.h>

#include "defines.h"
#include "http_server.h"

namespace captiverc {

#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
//  test_cb
//  Callback used to handle sending the control address response.
////////////////////////////////////////////////////////////////////////////////
void http_server::test_cb(struct evhttp_request *req, void *arg) {
    const char *cmdtype;
    struct evkeyvalq *headers;
    struct evkeyval *header;
    struct evbuffer *buf;

    switch (evhttp_request_get_command(req)) {
        case EVHTTP_REQ_GET:
            cmdtype = "GET";
            break;
        case EVHTTP_REQ_POST:
            cmdtype = "POST";
            break;
        case EVHTTP_REQ_HEAD:
            cmdtype = "HEAD";
            break;
        case EVHTTP_REQ_PUT:
            cmdtype = "PUT";
            break;
        case EVHTTP_REQ_DELETE:
            cmdtype = "DELETE";
            break;
        case EVHTTP_REQ_OPTIONS:
            cmdtype = "OPTIONS";
            break;
        case EVHTTP_REQ_TRACE:
            cmdtype = "TRACE";
            break;
        case EVHTTP_REQ_CONNECT:
            cmdtype = "CONNECT";
            break;
        case EVHTTP_REQ_PATCH:
            cmdtype = "PATCH";
            break;
        default:
            cmdtype = "unknown";
            break;
    }

    std::cout << "Received a " << cmdtype << " request for "
              << evhttp_request_get_uri(req) << std::endl;

    // this is debugging info that was in the original code.
    buf = evhttp_request_get_input_buffer(req);
    std::cout << "Input data: <<<";
    while (evbuffer_get_length(buf)) {
        int n;
        char cbuf[128];
        n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
        if (n > 0) std::cout.write(cbuf, n);
        //(void) fwrite(cbuf, 1, n, stdout);
    }
    std::cout << ">>>" << std::endl;

    // buffer for the response
    struct evbuffer *evb = evbuffer_new();

    evbuffer_add_printf(evb, "\"Test-Response as JSON string.\"");

    evhttp_add_header(evhttp_request_get_output_headers(req),
                      "Content-Type",
                      "application/json");

    evhttp_send_reply(req, 200, "OK", evb);
}
#endif

////////////////////////////////////////////////////////////////////////////////
//  not_found_cb
//  Return 'Not Found' for default handling
////////////////////////////////////////////////////////////////////////////////
void http_server::not_found_cb (struct evhttp_request *req, void *arg){
    evhttp_send_error(req, HTTP_NOTFOUND, "Not Found");
}

////////////////////////////////////////////////////////////////////////////////
/// get_payload
/// Parse the request's data into a string and return it.
////////////////////////////////////////////////////////////////////////////////
std::string http_server::get_payload(struct evhttp_request *req){
    struct evbuffer *buf;
    std::string payload;

    buf = evhttp_request_get_input_buffer(req);
    while (evbuffer_get_length(buf)) {
        char cbuf[128];
        int n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
        if (n > 0) {
            payload.append(cbuf,n);
        }
    }

    return payload;
}

////////////////////////////////////////////////////////////////////////////////
/// send_json_response
/// Send a json response
////////////////////////////////////////////////////////////////////////////////
void
http_server::send_json_response (struct evhttp_request *req, const char* response) {
    // buffer for the response
    struct evbuffer *evb = evbuffer_new();

    evbuffer_add_printf(evb, "%s", response);

    evhttp_add_header(evhttp_request_get_output_headers(req),
                      "Content-Type",
                      "application/json");

    evhttp_send_reply(req, 200, "OK", evb);
}



////////////////////////////////////////////////////////////////////////////////
// register_callback
////////////////////////////////////////////////////////////////////////////////
int
http_server::register_callback (std::string uri,
                                void (*cb)(struct evhttp_request *, void *), 
                                void* arg) {
    return evhttp_set_cb(httpd_.get(), uri.c_str(), cb, arg);
}


////////////////////////////////////////////////////////////////////////////////
// get_control_address
// Read the control address from the config file and return it as a string.
// If the address is not found, issue an error and loop.
////////////////////////////////////////////////////////////////////////////////
std::string
http_server::get_control_address(){
    std::string return_value;

    while (true) {
        std::ifstream infile(FILE_ENF_CONTROL_ADDRESS);
        if (infile.is_open()){
            std::getline(infile, return_value);
            infile.close();
            return return_value;
        }
        usleep (1000000);   // sleep for 1 sec
    }

    return "::1";       // dead code - return loopback

}


////////////////////////////////////////////////////////////////////////////////
//  Constructor
//
////////////////////////////////////////////////////////////////////////////////
http_server::http_server(const int port)
    : port_(port),
      base_(event_base_new(), &event_base_free),
      httpd_(evhttp_new(base_.get()), &evhttp_free),
      running_(false) {
    std::cout << "Starting up" << std::endl;

    evhttp_set_gencb(httpd_.get(), &not_found_cb, this);

#ifdef DEBUG
    // test using arg for the instance variable
    evhttp_set_cb(
            httpd_.get(),
            "/test",
            [](struct evhttp_request *req, void *arg) {
                http_server* that = static_cast<http_server*>(arg);
                that->test_cb(req, arg);
            },
            this);
#endif

    std::string ctrl_addr = get_control_address();

    if (int ret = evhttp_bind_socket(httpd_.get(), ctrl_addr.c_str(), port_) != 0) {
        std::cout << "Failed to bind to socket. Return code: " << ret
                  << std::endl;
        return;
    }

    running_ = true;

    std::cout << "Listening on port " << port_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Destructor
//
////////////////////////////////////////////////////////////////////////////////
http_server::~http_server() {
    std::cout << "Shutting down" << std::endl;

    running_ = false;
    std::cout << "Stopped" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  loop_dispatch
//
////////////////////////////////////////////////////////////////////////////////
void http_server::loop_dispatch() {
    const struct timeval one_sec = {1, 0};
    while (running_) {
        event_base_loopexit(base_.get(), &one_sec);
        event_base_dispatch(base_.get());
    }
}

}    // namespace captiverc
