#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "get_file.h"
#include "reboot.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_reboot::rest_reboot (std::string path, std::string reboot_exe):
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
rest_reboot::post(resource::req_type body){
    int result = system(reboot_exe_.c_str());

    if (0 == result){
        auto msg = "Reboot Scheduled";
        return std::make_tuple(HTTP_OK, json::string(msg));
    } else {
        std::stringstream ss;
        ss << "\"Received unexpected result code:" << result
           << " from Reboot request.\"";
        return std::make_tuple(HTTP_INTERNAL, json::string(ss));
    }
}

}   // namespace captiverc
