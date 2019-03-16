#include <iostream>
#include <fstream>
#include <sstream>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"
#include "rest/put_file.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
put_file::put_file (std::string path,
                    std::string filename):
            get_file(path, filename)
    {}

////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Changes the file and then returns its conents as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
put_file::put(resource::req_type body){
    json_t* root = body.get();

    // we should only be gettin a JSON string
    if (!json_is_string(root)) {
        auto msg = "Error: JSON should contain only a string.";
        return std::make_tuple(http::status::bad_request, json::string(msg));
    }

    std::string newval = json_string_value(root);

    std::ofstream outfile(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::stringstream temp_ss;
        temp_ss << "Error: unable to open: " << filename_
                  << " for writing new value." << std::endl;
        return std::make_tuple(http::status::internal_server_error, json::string(temp_ss));
    }

    outfile << newval << std::endl;

    outfile.close();

    return get(body);
}

} // namespace rest
} // namespace captiverc
