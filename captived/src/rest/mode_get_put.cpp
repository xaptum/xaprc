#include <iostream>
#include <fstream>
#include <sstream>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"
#include "rest/mode_get_put.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
mode_get_put::mode_get_put (std::string path,
                            std::string filename):
            get_file(path, filename)
    {}

////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Validates the mode data, changest the file, and then returns its conents 
/// as a JSON value.
/// 
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
mode_get_put::put(resource::req_type body){
    json_t* root = body.get();

    // we should only be gettin a JSON string
    if (!json_is_string(root)) {
        auto msg = "Error: JSON should contain only a string.";
        return bad_request(json::string(msg));
    }

    std::string newval = json_string_value(root);

    // validate values
    if ((newval != MODE_PASSTHROUGH) && (newval != MODE_SECURE_HOST)
            && (newval != MODE_SECURE_LAN)){
        auto msg = "Error - invalid value for router mode.";
        return bad_request(json::string(msg));
    }

    std::ofstream outfile(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::stringstream temp_ss;
        temp_ss << "Error: unable to open: " << filename_
                  << " for writing new value." << std::endl;
        return internal_server_error(json::string(temp_ss));
    }

    outfile << newval << std::endl;

    outfile.close();

    return get(body);
}

} // namespace rest
} // namespace captiverc
