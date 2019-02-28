#include <iostream>
#include <fstream>

#include <event2/http.h>

#include "defines.h"
#include "http_server.h"
#include "jansson.h"
#include "rest_resource_generic.h"
#include "rest_resource_root.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_resource_root::rest_resource_root (std::string uri,
                                        http_server* server):
            rest_resource_generic(uri, server){
    serverp_->register_callback(
            uri_,
            [](struct evhttp_request *req, void* arg) {
                rest_resource_root* that = 
                        static_cast<rest_resource_root*>(arg);
                that->callback(req, arg);
            },
            this);

}

////////////////////////////////////////////////////////////////////////////////
/// callback
/// Implement the pure virtual callback function
/// This will be called from the lambda expression, so it will have an object
/// reference
/// Callback used to send all available status information.
////////////////////////////////////////////////////////////////////////////////
// TODO - do we need the *arg here?
void
rest_resource_root::callback(struct evhttp_request *req, void *arg){
    const char *cmdtype;
    struct evkeyvalq *headers;
    struct evkeyval *header;

    // we only handle GET  -- return an error otherwise.
    if (!serverp_->is_get_request(req)) {
            respond_op_not_allowed(req);
            return;
    }

    // buffer for the response
    struct evbuffer *evb = evbuffer_new();

    std::string json = get_status();

    serverp_->send_json_response(req, json.c_str());
 
}

////////////////////////////////////////////////////////////////////////////////
/// get_status
/// Get the top-level status of the router as a JSON file.
////////////////////////////////////////////////////////////////////////////////
std::string rest_resource_root::get_status() {

    json_t* root = json_object();

    json_object_set_new(root, 
                        "serial_number", 
                        json_string(get_file_contents(FILE_SERIAL_NUMBER).c_str()));
    json_object_set_new(root, 
                        "firmware_version", 
                        json_string(get_file_contents(FILE_FIRMWARE_VERSION).c_str()));
    json_object_set_new(root, 
                        "mac_address", 
                        json_string(get_file_contents(FILE_WIFI_MAC_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "control_address", 
                        json_string(get_file_contents(FILE_ENF_CONTROL_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "data_address", 
                        json_string(get_file_contents(FILE_ENF_DATA_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "mode", 
                        json_string(get_file_contents(FILE_ROUTER_MODE).c_str()));

    char* zsjson = json_dumps(root, JSON_INDENT(2));
    std::string status_json{zsjson};

    free (zsjson);
    json_decref(root);
    
    return status_json;
}


////////////////////////////////////////////////////////////////////////////////
/// get_file_contents
/// Read the first line of the file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string rest_resource_root::get_file_contents(std::string filename) {
    std::string return_value;

    std::ifstream infile(filename);
    if (infile.is_open()) {
        std::getline(infile, return_value);
        infile.close();
    } else {
        return_value = "NOT FOUND";
    }

    return return_value;
}


}   // namespace captiverc
