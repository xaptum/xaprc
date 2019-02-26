#include <iostream>
#include <fstream>

#include <event2/http.h>

#include "http_server.h"
#include "rest_resource_generic.h"

namespace captiverc {

rest_resource_generic::rest_resource_generic(std::string uri,
                                             http_server* server):
            uri_(uri),
            serverp_(server)
    {}


std::string
rest_resource_generic::req_type_string(enum evhttp_cmd_type req_type) {
    std::string cmdtype;

    switch (req_type) {
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

    return cmdtype;
}

void 
rest_resource_generic::respond_op_not_allowed(struct evhttp_request* req) {
    std::string err = req_type_string(evhttp_request_get_command(req));
    err += " operation not allowed for URI:";
    err += uri_;
    serverp_->respond_not_allowed(req, err);
    return;
}

}   // namespace captiverc