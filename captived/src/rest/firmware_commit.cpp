#include "rest/firmware_commit.hpp"
#include "rest/resource.hpp"

namespace captived {
namespace rest {

firmware_commit::firmware_commit(std::string path,
                                 system& system,
                                 firmware_manager& fw_mgr,
                                 std::string commit_exe,
                                 firmware& fw)
    : resource(path),
      system_(system),
      fw_mgr_(fw_mgr),
      commit_exe_(commit_exe),
      fw_(fw) {}

int
firmware_commit::execute() {
    return system_.execute(commit_exe_);
}

firmware_commit::resp_type
firmware_commit::put(req_type body) {
    // Check if device is in "normal" or "uncommitted" states.
    auto state = fw_mgr_.firmware_state();
    if (!state) {
        auto msg =
            "Firmware cannot be committed because its state cannot be found.";
        return bad_request(json::string(msg));
    }

    if (*state != FIRMWARE_STATE_NORMAL &&
        *state != FIRMWARE_STATE_UNCOMMITTED) {
        auto msg = "Firmware cannot be committed in state: " + *state;
        return bad_request(json::string(msg));
    }

    int result = execute();

    if (0 == result) {
        auto empty_req = req_type();
        return fw_.get(empty_req);
    } else {
        std::stringstream ss;
        ss << "Failed to firmware_commit with error code: " << result;
        return internal_server_error(json::string(ss));
    }
}

}    // namespace rest
}    // namespace captived
