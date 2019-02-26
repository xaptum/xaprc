#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>

#include <boost/variant.hpp>

#include <event2/buffer.h>
#include <jansson.h>
#include "defines.h"
#include "http_server.h"

namespace captiverc {

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

////////////////////////////////////////////////////////////////////////////////
//  not_found_cb
//  Return 'Not Found' for default handling
////////////////////////////////////////////////////////////////////////////////
void http_server::not_found_cb (struct evhttp_request *req, void *arg){
    evhttp_send_error(req, HTTP_NOTFOUND, "Not Found");
}


////////////////////////////////////////////////////////////////////////////////
//  full_status_cb
//  Callback used to send all available status information.
////////////////////////////////////////////////////////////////////////////////
// void http_server::full_status_cb(struct evhttp_request *req, void *arg) {
//     const char *cmdtype;
//     struct evkeyvalq *headers;
//     struct evkeyval *header;
//     struct evbuffer *buf;

//     // we don't care what type of http request this is... we'll just return the
//     // value as if it were a "get"

//     // clear the input buffer if we were sent data
//     // Do we need to do this?  Docs were not really clear.
//     buf = evhttp_request_get_input_buffer(req);
//     while (evbuffer_get_length(buf)) {
//         int n = evbuffer_drain(buf, evbuffer_get_length(buf));
//         if (n < 0)
//             std::cout << "Error occurred while emptying input buffer"
//                       << std::endl;
//     }

//     // buffer for the response
//     struct evbuffer *evb = evbuffer_new();

//     std::string json = get_status();
//     evbuffer_add(evb, json.c_str(), json.length());

//     evhttp_add_header(evhttp_request_get_output_headers(req),
//                       "Content-Type",
//                       "application/json");

//     evhttp_send_reply(req, 200, "OK", evb);
// }




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
            payload += cbuf;
        }
    }

    return payload;
}

////////////////////////////////////////////////////////////////////////////////
/// send_json_response
/// Send a json response
////////////////////////////////////////////////////////////////////////////////
void
http_server::send_json_response (struct evhttp_request *req, char* response) {
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

    //evhttp_set_cb(httpd_.get(), "/test", test_cb, NULL);

    // test using arg for the instance variable
    evhttp_set_cb(
            httpd_.get(),
            "/test",
            [](struct evhttp_request *req, void *arg) {
                http_server* that = static_cast<http_server*>(arg);
                that->test_cb(req, arg);
            },
            this);


    // evhttp_set_cb(httpd_.get(), "/", full_status_cb, NULL);

    if (int ret = evhttp_bind_socket(httpd_.get(), "127.0.0.1", port_) != 0) {
        std::cout << "Failed to bind to socket. Return code: " << ret
                  << std::endl;
        return;
    }

    running_ = true;
    // don't run on another thread - run from wherever constructed this
    // event_thread_ = std::thread(&http_server::loop_dispatch, this);

    std::cout << "Listening on port " << port_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Destructor
//
////////////////////////////////////////////////////////////////////////////////
http_server::~http_server() {
    std::cout << "Shutting down" << std::endl;

    running_ = false;
    // event_thread_.join();

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

////////////////////////////////////////////////////////////////////////////////
/// get_status
/// Get the top-level status of the router as a JSON file.
////////////////////////////////////////////////////////////////////////////////
// std::string http_server::get_status() {
//     std::string status_json = "{\n";

//     status_json += "\t\"serial_number\" : \"" +
//                    get_file_contents(FILE_SERIAL_NUMBER) + "\",\n";
//     status_json += "\t\"firmware_version\" : \"" +
//                    get_file_contents(FILE_FIRMWARE_VERSION) + "\",\n";
//     status_json += "\t\"mac_address\" : \"" +
//                    get_file_contents(FILE_WIFI_MAC_ADDRESS) + "\",\n";
//     status_json += "\t\"control_address\" : \"" +
//                    get_file_contents(FILE_ENF_CONTROL_ADDRESS) + "\",\n";

//     status_json += "\t\"data_address\" : \"" +
//                    get_file_contents(FILE_ENF_DATA_ADDRESS) + "\",\n";

//     status_json +=
//             "\t\"mode\" : \"" + get_file_contents(FILE_ROUTER_MODE) + "\"\n";

//     status_json += "}\n";

//     return status_json;
// }





}    // namespace captiverc
