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
rest_wifi_config::get(resource::req_type body) {
    auto root = json::object();
    json::object_set(root, "contents", json::string(get_entire_file()));
    return std::make_tuple(HTTP_OK, root);
}


////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Changest the file and then returns its conents as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_wifi_config::put(resource::req_type body){
    json_t* root = body.get();

    // we should only be gettin a JSON object
    if (!json_is_object(root)){
        auto msg = "Error: JSON must contain only an object.";
        return std::make_tuple(HTTP_BADREQUEST, json::string(msg));
    }

    json_t* contents = json_object_get(root, "contents");
    if (!json_is_string(contents)){
        auto msg = "Error: 'conents' element must be a string.";
        return std::make_tuple(HTTP_BADREQUEST, json::string(msg));
    }

    std::string newval = json_string_value(contents);

    std::ofstream outfile(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::stringstream temp_ss;
        temp_ss << "Error: unable to open: " << filename_
                << " for writing new value." << std::endl;
        return std::make_tuple(HTTP_INTERNAL, json::string(temp_ss));
    }

    outfile << newval << std::endl;

    outfile.close();

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
