#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"
#include "rest/reboot.hpp"

namespace captiverc {
namespace rest {

reboot::reboot(std::string path, std::string reboot_exe) :
    resource(path),
    reboot_exe_(reboot_exe)
{}

int
reboot::execute() {
    return system(reboot_exe_.c_str());
}

reboot::resp_type
reboot::post(req_type body) {
    int result = execute();

    if (0 == result) {
        auto msg = "Reboot Scheduled";
        return ok(json::string(msg));
    } else {
        std::stringstream ss;
        ss << "Failed to reboot with error code " << result;
        return internal_server_error(json::string(ss));
    }
}

} // namespace rest
} // namespace captiverc
