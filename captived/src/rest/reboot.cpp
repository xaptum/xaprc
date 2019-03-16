#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"
#include "rest/reboot.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
reboot::reboot (std::string path, std::string reboot_exe):
            resource(path),
            reboot_exe_(reboot_exe)
    {}

////////////////////////////////////////////////////////////////////////////////
/// post
/// Implement the 'post' functionality
/// Schedules a reboot and then sends a response.
/// The system should have enough time for to respond before the reboot
/// acctually occurs.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
reboot::post(resource::req_type body){
    int result = system(reboot_exe_.c_str());

    if (0 == result){
        auto msg = "Reboot Scheduled";
        return std::make_tuple(http::status::ok, json::string(msg));
    } else {
        std::stringstream ss;
        ss << "\"Received unexpected result code:" << result
           << " from Reboot request.\"";
        return std::make_tuple(http::status::internal_server_error, json::string(ss));
    }
}

} // namespace restn
} // namespace captiverc
