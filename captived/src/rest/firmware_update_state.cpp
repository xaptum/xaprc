#include "rest/firmware_update_state.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

firmware_update_state::firmware_update_state(std::string path,
                                             firmware_manager& fw_mgr)
    : resource(path), fw_mgr_(fw_mgr) {}

/******************************************************************************
 * get
 ******************************************************************************/
firmware_update_state::resp_type
firmware_update_state::get(resource::req_type) {
    auto fw_state = fw_mgr_.firmware_state();
    if (!fw_state) {
        auto msg = "Error: failed to read resource: update_state";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*fw_state));
}

}    // namespace rest
}    // namespace captived
