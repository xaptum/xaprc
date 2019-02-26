#include <iostream>
#include <fstream>

#include <event2/http.h>

#include "http_server.h"
#include "jansson.h"
#include "rest_resource_generic.h"
#include "rest_resource_to_file.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_resource_to_file::rest_resource_to_file (std::string uri,
                                              http_server* server,
                                              std::string filename,
                                              bool allow_put = false):
            rest_resource_generic(uri, server),
            filename_(filename),
            allow_put_(allow_put){
    serverp_->register_callback(
            uri_,
            [](struct evhttp_request *req, void* arg) {
                rest_resource_to_file* that = static_cast<rest_resource_to_file*>(arg);
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
rest_resource_to_file::callback(struct evhttp_request *req, void *arg){


    const char *cmdtype;
    struct evkeyvalq *headers;
    struct evkeyval *header;
    

    // we only handle GET and PUT -- return an error otherwise.
    if ((!serverp_->is_get_request(req))
        && ((!serverp_->is_put_request(req)) || (!allow_put_))
       ){
            respond_op_not_allowed(req);
            std::string err = req_type_string(evhttp_request_get_command(req));
            err += " operation not allowed for URI:";
            err += uri_;
            serverp_->respond_not_allowed (req, err);
            return;
    }

    std::string payload = serverp_->get_payload(req);

    if (serverp_->is_put_request(req)){
        int result = set_file_contents(payload);
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
rest_resource_to_file::set_file_contents(std::string payload) {
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
std::string rest_resource_to_file::get_file_contents() {
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