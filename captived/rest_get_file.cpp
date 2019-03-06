#include <iostream>
#include <fstream>
#include <sstream>

#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "rest_get_file.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_get_file::rest_get_file (std::string path,
                              std::string filename):
            resource(path),
            filename_(filename)
    {}

////////////////////////////////////////////////////////////////////////////////
/// get
/// Implement the 'get' functionality
/// Return the conents of the file as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_get_file::get(resource::req_type body){
    json_t* json_resp = json_pack("s", get_file_contents().c_str());
    char* text_resp = json_dumps(json_resp, JSON_ENCODE_ANY | JSON_INDENT(2));

    std::string json_string = text_resp;

    free(text_resp);
    json_decref(json_resp);
 
    return std::make_tuple(HTTP_OK, json_string);
}

////////////////////////////////////////////////////////////////////////////////
/// get_file_contents
/// Read the first line of the file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string rest_get_file::get_file_contents() {
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
