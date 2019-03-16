#include <iostream>
#include <fstream>
#include <tuple>
#include <event2/http.h>

#include "defines.hpp"
#include "rest/resource.hpp"
#include "rest/root.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
root::root (std::string root_path):
            resource("/"),
            root_path_(root_path)
        {}


////////////////////////////////////////////////////////////////////////////////
/// get
/// Implement the root 'get' functionality
/// Get the top-level status of the router as a JSON file.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
root::get(resource::req_type body) {
    auto root = json::object();

    json::object_set(root,
                     "serial_number",
                     json::string(get_file_contents(root_path_ + FILE_SERIAL_NUMBER)));
    json::object_set(root,
                     "firmware_version",
                     json::string(get_file_contents(root_path_ + FILE_FIRMWARE_VERSION)));
    json::object_set(root,
                     "mac_address",
                     json::string(get_file_contents(root_path_ + FILE_WIFI_MAC_ADDRESS)));
    json::object_set(root,
                     "control_address",
                     json::string(get_file_contents(root_path_ + FILE_ENF_CONTROL_ADDRESS)));
    json::object_set(root,
                     "data_address",
                     json::string(get_file_contents(root_path_ + FILE_ENF_DATA_ADDRESS)));
    json::object_set(root,
                     "mode",
                     json::string(get_file_contents(root_path_ + FILE_ROUTER_MODE)));

    return ok(root);
}


////////////////////////////////////////////////////////////////////////////////
/// get_file_contents
/// Read the first line of the file specified in the member variable and 
// return it as a string.
////////////////////////////////////////////////////////////////////////////////
std::string root::get_file_contents(std::string filename) {
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

} // namespace rest
} // namespace captiverc
