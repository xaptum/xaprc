#include <iostream>
#include <fstream>

#include <event2/http.h>

#include "defines.h"
#include "http_server.h"
#include "jansson.h"
#include "rest_resource_generic.h"
#include "rest_resource_router_mode.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_resource_router_mode::rest_resource_router_mode (std::string uri,
                                              http_server* server,
                                              std::string filename):
            rest_resource_generic(uri, server),
            filename_(filename){
    serverp_->register_callback(
            uri_,
            [](struct evhttp_request *req, void* arg) {
                rest_resource_router_mode* that = 
                        static_cast<rest_resource_router_mode*>(arg);
                that->callback(req, arg);
            },
            this);

}

////////////////////////////////////////////////////////////////////////////////
/// callback
/// Implement the pure virtual callback function
/// This will be called from the lambda expression, so it will have an object
/// reference
////////////////////////////////////////////////////////////////////////////////
// TODO - do we need the *arg here?
void
rest_resource_router_mode::callback(struct evhttp_request *req, void *arg){


    const char *cmdtype;
    struct evkeyvalq *headers;
    struct evkeyval *header;
    

    // we only handle GET and PUT -- return an error otherwise.
    if ((!serverp_->is_get_request(req))
        && (!serverp_->is_put_request(req))
       ){
            respond_op_not_allowed(req);
            return;
    }

    std::string payload = serverp_->get_payload(req);

    if (serverp_->is_put_request(req)){
        int result = set_file_contents(payload);
        if (result < 0){
            serverp_->respond_bad_request(req, "Error: Received invalid data.");
            return;
        }
    }

    json_t* json_resp = json_pack("s", get_file_contents().c_str());
    char* text_resp = json_dumps(json_resp, JSON_ENCODE_ANY | JSON_INDENT(2));
    serverp_->send_json_response(req, text_resp);

    free(text_resp);
    json_decref(json_resp);
 
}

////////////////////////////////////////////////////////////////////////////////
/// set_file_contents
/// Read the first line of the specified file and return it as a string.
////////////////////////////////////////////////////////////////////////////////
// TODO - consider returning an error message to be returned vi HTTP response
int
rest_resource_router_mode::set_file_contents(std::string payload) {
    //parse the json
    json_t* root;
    json_error_t error;

    std::string temp_val = payload;
    std::cout << "The JSON payload is: " << temp_val << std::endl;
    root = json_loads(payload.c_str(), JSON_DECODE_ANY, &error);
    if (!root){
        std::cerr << "Error: on line " << error.line << ": " << error.text
                  << std::endl;
        json_decref(root);
        return -1;
    }

    // we should only be gettin a JSON string
    if (!json_is_string(root)){
        std::cerr << "Error: JSON should contain only a string." << std::endl;
        json_decref(root);
        return -1;
    }
    
    std::string newval = json_string_value(root);
    if ((newval != MODE_PASSTHROUGH) && (newval != MODE_SECURE_HOST)
            && (newval != MODE_SECURE_LAN)){
        std::cerr << "Error - invalid value for router mode." << std::endl;
        json_decref(root);
        return -1;
    } 

    std::ofstream outfile(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::cerr << "Error: unable to open: " << filename_ 
                  << " for writing new value." << std::endl;
        json_decref(root);
        return -1;
    }

    outfile << newval << std::endl;

    outfile.close();

    json_decref(root);
    return 0;

}

////////////////////////////////////////////////////////////////////////////////
/// get_file_contents
/// Read the first line of the file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string rest_resource_router_mode::get_file_contents() {
    std::string return_value;

    std::ifstream infile(filename_);
    if (infile.is_open()) {
        std::getline(infile, return_value);
        infile.close();
    } else {
        return_value = "NOT FOUND";
    }

    return return_value;
}


}   // namespace captiverc
