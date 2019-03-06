#include <iostream>
#include <fstream>
#include <sstream>

#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "rest_get_file.h"
#include "rest_put_file.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_put_file::rest_put_file (std::string path,
                              std::string filename):
            rest_get_file(path, filename)
    {}

////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Changes the file and then returns its conents as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_put_file::put(resource::req_type body){
    //parse the json
    json_t* root;
    json_error_t error;

    std::cout << "The JSON payload is: " << body << std::endl;
    root = json_loads(body.c_str(), JSON_DECODE_ANY, &error);
    if (!root){
        std::stringstream temp_ss;
        temp_ss << "\"JSON parsing error on line"<< error.line << ": " 
                << error.text << "\"" << std::endl;
        json_decref(root);
        return std::make_tuple(HTTP_BADREQUEST, temp_ss.str());
    }

    // we should only be gettin a JSON string
    if (!json_is_string(root)){
        json_decref(root);
        return std::make_tuple(HTTP_BADREQUEST, 
                               "\"Error: JSON should contain only a string.\"");
    }
    
    std::string newval = json_string_value(root);

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

}   // namespace captiverc