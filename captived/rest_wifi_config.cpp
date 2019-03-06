#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "rest_wifi_config.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_wifi_config::rest_wifi_config (std::string path,
                              std::string filename):
            resource(path),
            filename_(filename)
    {}

////////////////////////////////////////////////////////////////////////////////
/// get
/// Implement the 'get' functionality
/// Return the conents of the file as a member of a JSON object.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_wifi_config::get(resource::req_type body){
    json_t* json_resp = json_object();

    json_object_set_new(json_resp,
                        "contents",
                        json_string(get_entire_file().c_str()));
    char* text_resp = json_dumps(json_resp, JSON_INDENT(2));
    std::string json_string = text_resp;
    
    free(text_resp);
    json_decref(json_resp);

    return std::make_tuple(HTTP_OK, json_string);
}


////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Changest the file and then returns its conents as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_wifi_config::put(resource::req_type body){
    //parse the json
    json_t* root;
    json_error_t error;

    std::cout << "Wifi-config JSON payload is: <<<" << body << ">>>" <<std::endl;
    root = json_loads(body.c_str(), 0, &error);
    if (!root){
        std::stringstream temp_ss;
        temp_ss << "\"JSON parsing error on line"<< error.line << ": " 
                << error.text << "\"" << std::endl;
        json_decref(root);
        return std::make_tuple(HTTP_BADREQUEST, temp_ss.str());
    }

    // we should only be gettin a JSON object
    if (!json_is_object(root)){
        json_decref(root);
        return std::make_tuple(HTTP_BADREQUEST, 
                            "\"Error: JSON must contain only an object.\"");
    }
    
    json_t* contents = json_object_get(root, "contents");
    if (!json_is_string(contents)){
        json_decref(root);
        return std::make_tuple(HTTP_BADREQUEST, 
                            "\"Error: 'conents' element must be a string.\"");
    }

    std::string newval = json_string_value(contents);

    std::ofstream outfile(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::stringstream temp_ss;
        temp_ss << "\"Error: unable to open: " << filename_ 
                  << " for writing new value.\"" << std::endl;
        json_decref(root);
        return std::make_tuple(HTTP_INTERNAL, temp_ss.str());
    }

    outfile << newval << std::endl;

    outfile.close();

    json_decref(root);
    return get(body);
}

////////////////////////////////////////////////////////////////////////////////
/// get_entire_file
/// Read the entire file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string rest_wifi_config::get_entire_file() {
    std::ifstream infile(filename_);
    std::stringstream buffer;
    if (infile.is_open()) {
        buffer << infile.rdbuf();
        infile.close();
    } else {
        buffer << "NOT FOUND";
    }

    return buffer.str();
}

}   // namespace captiverc
