#include <algorithm>
#include <event2/buffer.h>

#include "defines.h"
#include "http_server.h"

namespace captiverc {



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
http_server::send_json_response (struct evhttp_request *req, 
                                 resource::resp_type response) {
    int resp_code = std::get<0>(response);
    std::string resp_text = std::get<1>(response);

    // buffer for the response
    struct evbuffer *evb = evbuffer_new();

    evbuffer_add_printf(evb, "%s", resp_text.c_str());

    evhttp_add_header(evhttp_request_get_output_headers(req),
                      "Content-Type",
                      "application/json");

    evhttp_send_reply(req, resp_code, "OK", evb);

    evbuffer_free(evb);
}


////////////////////////////////////////////////////////////////////////////////
// get_control_address
// Read the control address from the config file and return it as a string.
// If the address is not found, issue an error and loop.
////////////////////////////////////////////////////////////////////////////////
std::string
http_server::get_control_address(){
    std::string return_value;
    std::string fq_filename = root_path_ + FILE_ENF_CONTROL_ADDRESS;

    while (true) {
        std::ifstream infile(fq_filename);
        if (infile.is_open()){
            std::getline(infile, return_value);
            infile.close();
            return return_value;
        }
        usleep (1000000);   // sleep for 1 sec
        std::cerr << "Failed to get server address from: " 
                  << fq_filename << std::endl;
    }

    return "::1";       // dead code - return loopback

}


////////////////////////////////////////////////////////////////////////////////
//  Constructor
//
////////////////////////////////////////////////////////////////////////////////
http_server::http_server(const int port, const std::string root_path)
    : port_(port),
      root_path_(root_path),
      base_(event_base_new(), &event_base_free),
      httpd_(evhttp_new(base_.get()), &evhttp_free),
      running_(false) {
    std::cout << "Starting up" << std::endl;

    evhttp_set_gencb(httpd_.get(), &not_found_cb, this);

    bool bound_to_socket = false;

    // keep looping until a connection is established
    do {
        std::string ctrl_addr = get_control_address();

        if (int ret = evhttp_bind_socket(
                              httpd_.get(), ctrl_addr.c_str(), port_) != 0) {
            std::cout << "Failed to bind to socket. Return code: " << ret
                      << std::endl;
            bound_to_socket = false;
            usleep (1000000);   // sleep for 1 sec

        } else {
            bound_to_socket = true;
        }

    } while (false == bound_to_socket);

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
