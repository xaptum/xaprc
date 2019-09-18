#include "rest/mode.hpp"

#include "defines.hpp"

namespace captived {

namespace {
bool
is_valid_mode(std::string mode) {
    /*
     * As a safeguard, allow only the modes actually supported by the firmware.
     */
    // return (mode == MODE_PASSTHROUGH || mode == MODE_SECURE_HOST ||
    //        mode == MODE_SECURE_LAN);
    return (mode == MODE_SECURE_HOST);
}
}    // namespace

namespace rest {

mode::mode(std::string path,
           system& system,
           std::string target_path,
           std::string mode_symlink,
           reboot reboot_resource)
    : resource(path),
      system_(system),
      mode_symlink_(mode_symlink),
      target_path_(target_path),
      reboot_resource_(reboot_resource) {}

/**
 * get
 * Handles the get operation on the /mode REST resource.
 */
mode::resp_type
mode::get(resource::req_type) {
    auto temp_mode = router_mode();
    if (!temp_mode) {
        auto msg = "Error: failed to find router mode.";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*temp_mode));
}

/**
 * put
 * Handles the 'put' operation on the /mode REST resource
 */
mode::resp_type
mode::put(resource::req_type body) {
    // Parse and validate the request body
    json_t* root = body.get();

    if (!json_is_string(root)) {
        auto msg = "Error: request body is not a JSON string.";
        return bad_request(json::string(msg));
    }

    std::string newval = json_string_value(root);

    if (!is_valid_mode(newval)) {
        auto msg = "Error: mode is invalid.";
        return bad_request(json::string(msg));
    }

    if (router_mode(newval) == false) {
        auto msg = "Error: failed to update mode.";
        return internal_server_error(json::string(msg));
    }

    auto temp_mode = router_mode_config();
    if (!temp_mode) {
        auto msg = "Error: failed to find router mode.";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*temp_mode));
}

/**
 * Returns the configured mode of the router card.
 * Will be one of "passthrough", "secure-host", or "secure-lan".
 *
 * @returns The router card mode or None on an error.
 */
std::experimental::optional<std::string>
mode::router_mode_config() {
    auto fq_link = system_.symlink_target(mode_symlink_);
    if (!fq_link) {
        return {};
    }

    size_t period = fq_link->find_last_of('.');
    size_t slash = fq_link->find_last_of('/');
    std::string mode = fq_link->substr(slash + 1, period - slash - 1);
    return {mode};
}

/**
 * Sets the router card's configured mode.
 * It is up to caller to reboot the router to apply changes.
 *
 * @param new_mode The mode to set. Valid input modes are: "passthrough",
 *     "secure-host", or "secure-lan".
 * @returns true on success, false otherwise.
 */
bool
mode::router_mode_config(std::string new_mode) {
    std::string prev_mode_str;

    if (!is_valid_mode(new_mode)) {
        return false;
    }

    std::string target_name = target_path_ + "/" + new_mode + ".target";
    bool linked = system_.symlink_target(target_name, mode_symlink_);

    if (!linked) {
        return false;
    }
    return true;
}

/**
 * Returns the currently running mode of the router card.
 * This might be different from the configured mode if the card
 * has not been rebooted yet.
 * Will be one of "passthrough", "secure-host", or "secure-lan".
 *
 * @returns The router card mode or None on an error.
 */
std::experimental::optional<std::string>
mode::router_mode() {
    std::string result;
    int ret_code = system_.execute(COMMAND_GET_ACTIVE_TARGETS, result);
    if (ret_code != 0) {
        return {};
    }

    std::regex re("(passthrough|secure-host|secure-lan)\\.target");

    std::smatch match;
    std::regex_search(result, match, re);
    if (match.size() < 1)
        return {};

    auto cur_mode = match.str(1);

    return {cur_mode};
}

/**
 * Sets the router card's configured mode.
 *
 * If the new mode is different from either the previously configured mode or
 * currently running mode, then this will reboot the card.
 *
 * @param new_mode The mode to set. Valid input modes are: "passthrough",
 *     "secure-host", or "secure-lan".
 * @returns true on success, false otherwise.
 */
bool
mode::router_mode(std::string new_mode) {
    // validate the provided mode
    if (!is_valid_mode(new_mode))
        return false;

    if (!router_mode_config(new_mode)) {
        return false;
    }

    // reboot to apply the new mode, if needed
    if (new_mode != router_mode()) {
        int rc = reboot_resource_.execute();
        if (rc != 0)
            return false;
    }

    return true;
}

}    // namespace rest
}    // namespace captived
