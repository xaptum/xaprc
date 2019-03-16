#include <iostream>
#include <fstream>
#include <sstream>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
get_file::get_file (std::string path,
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
get_file::get(resource::req_type body){
    auto contents = get_file_contents();
    return std::make_tuple(http::status::ok, json::string(contents));
}

////////////////////////////////////////////////////////////////////////////////
/// get_file_contents
/// Read the first line of the file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string get_file::get_file_contents() {
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

} // namespace rest
} // namespace captiverc
