#include "rest/resource.hpp"
#include "rest/reboot.hpp"

namespace captiverc {
namespace rest {

reboot::reboot(std::string path, system system, std::string reboot_exe) :
    resource(path),
    system_(system),
    reboot_exe_(reboot_exe)
{}

int
reboot::execute() {
    return system_.execute(reboot_exe_);
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
