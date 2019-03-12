#include <iostream>
#include <fstream>
#include <tuple>
#include <event2/http.h>

#include "defines.h"
#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "rest_resource_root.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_resource_root::rest_resource_root (std::string root_path):
            resource("/"),
            root_path_(root_path)
        {}


////////////////////////////////////////////////////////////////////////////////
/// get
/// Implement the root 'get' functionality
/// Get the top-level status of the router as a JSON file.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_resource_root::get(resource::req_type body) {

    json_t* root = json_object();

    json_object_set_new(root, 
                        "serial_number", 
                        json_string(get_file_contents(root_path_ + FILE_SERIAL_NUMBER).c_str()));
    json_object_set_new(root, 
                        "firmware_version", 
                        json_string(get_file_contents(root_path_ + FILE_FIRMWARE_VERSION).c_str()));
    json_object_set_new(root, 
                        "mac_address", 
                        json_string(get_file_contents(root_path_ + FILE_WIFI_MAC_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "control_address", 
                        json_string(get_file_contents(root_path_ + FILE_ENF_CONTROL_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "data_address", 
                        json_string(get_file_contents(root_path_ + FILE_ENF_DATA_ADDRESS).c_str()));
    json_object_set_new(root, 
                        "mode", 
                        json_string(get_file_contents(root_path_ + FILE_ROUTER_MODE).c_str()));

    char* zsjson = json_dumps(root, JSON_INDENT(2));
    std::string status_json{zsjson};

    free (zsjson);
    json_decref(root);
    
    return std::make_tuple(HTTP_OK, status_json);
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
