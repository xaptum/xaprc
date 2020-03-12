#include "rest/firmware.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

/******************************************************************************
 * constructor
 ******************************************************************************/
firmware::firmware(std::string path,
                   system& system,
                   firmware_manager& fw_mgr,
                   firmware_uri& fw_uri)
    : aggregate_resource(path),
      system_(system),
      fw_mgr_(fw_mgr),
      fw_uri_(fw_uri) {
    add("update_URI", fw_uri, true);
}

/******************************************************************************
 * put
 ******************************************************************************/
firmware::resp_type
firmware::put(req_type body) {
    // request should be json string holding URI
    json_t* root = body.get();

    auto fw_state = fw_mgr_.firmware_state();
    if (!fw_state) {
        auto msg =
            "Error: server is unable to determine if an update is in progress.";
        return internal_server_error(json::string(msg));
    } else if (*fw_state != FIRMWARE_STATE_NORMAL) {
        auto msg = "Error: a previous update is in progress.";
        return bad_request(json::string(msg));
    }

    if (!json_is_object(root)) {
        auto msg = "Error: request body is not a JSON object.";
        return bad_request(json::string(msg));
    }

    auto update_URI = json_object_get(root, "update_URI");
    if (NULL == update_URI) {
        auto msg = "Error: 'update_URI' element is missing.";
        return bad_request(json::string(msg));
    }

    if (!json_is_string(update_URI)) {
        auto msg = "Error: 'update_URI' value must be a string.";
        return bad_request(json::string(msg));
    }

    std::string uri = json_string_value(update_URI);

    fw_uri_.uri(uri);

    int pid = fw_mgr_.download(uri);
    if (pid <= 0) {
        auto msg =
            "Error: system encountered an error attempting to download "
            "firmware.";
        return internal_server_error(json::string(msg));
    }

    return get(json::null());
}

}    // namespace rest
}    // namespace captived
