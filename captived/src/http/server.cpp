#include <algorithm>
#include <event2/buffer.h>

#include "defines.hpp"
#include "http/server.hpp"
#include "json.hpp"

namespace captived {
namespace http {

////////////////////////////////////////////////////////////////////////////////
//  not_found_cb
//  Return 'Not Found' for default handling
////////////////////////////////////////////////////////////////////////////////
void
server::not_found_cb(struct evhttp_request* req, void* arg) {
    evhttp_send_error(req, HTTP_NOTFOUND, "Not Found");
}

////////////////////////////////////////////////////////////////////////////////
/// get_payload
/// Parse the request's data into a string and return it.
////////////////////////////////////////////////////////////////////////////////
std::string
server::get_payload(struct evhttp_request* req) {
    struct evbuffer* buf;
    std::string payload;

    buf = evhttp_request_get_input_buffer(req);
    while (evbuffer_get_length(buf)) {
        char cbuf[128];
        int n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
        if (n > 0) {
            payload.append(cbuf, n);
        }
    }

    return payload;
}

////////////////////////////////////////////////////////////////////////////////
/// send_json_response
/// Send a json response
////////////////////////////////////////////////////////////////////////////////
void
server::send_json_response(struct evhttp_request* req,
                           rest::resource::resp_type response) {
    auto resp_code = std::get<0>(response);
    auto body = std::get<1>(response);

    auto encoded = json::dumps(body);

    // buffer for the response
    struct evbuffer* evb = evbuffer_new();

    evbuffer_add_printf(evb, "%s", encoded.c_str());

    evhttp_add_header(evhttp_request_get_output_headers(req),
                      "Content-Type",
                      "application/json");

    evhttp_send_reply(req, static_cast<unsigned>(resp_code), "OK", evb);

    evbuffer_free(evb);
}

////////////////////////////////////////////////////////////////////////////////
// get_control_address
// Read the control address from the config file and return it as a string.
// If the address is not found, issue an error and loop.
////////////////////////////////////////////////////////////////////////////////
std::string
server::get_control_address() {
    std::string return_value;
    std::string fq_filename = root_path_ + FILE_ENF_CONTROL_ADDRESS;

    while (true) {
        std::ifstream infile(fq_filename);
        if (infile.is_open()) {
            std::getline(infile, return_value);
            infile.close();
            return return_value;
        }
        std::cerr << "Failed to get server address from: " << fq_filename
                  << std::endl;
        usleep(1000000);    // sleep for 1 sec
    }

    return "::1";    // dead code - return loopback
}

////////////////////////////////////////////////////////////////////////////////
//  Constructor
//
////////////////////////////////////////////////////////////////////////////////
server::server(const int port,
               const std::string root_path,
               std::shared_ptr<event_base> base)
    : port_(port),
      root_path_(root_path),
      base_(base),
      httpd_(evhttp_new(base.get()), &evhttp_free) {
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
            usleep(1000000);    // sleep for 1 sec

        } else {
            bound_to_socket = true;
        }

    } while (false == bound_to_socket);

    std::cout << "Listening on port " << port_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Destructor
//
////////////////////////////////////////////////////////////////////////////////
server::~server() { std::cout << "Server shutting down" << std::endl; }

}    // namespace http
}    // namespace captived
