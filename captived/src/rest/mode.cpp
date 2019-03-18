#include "rest/mode.hpp"

#include "defines.hpp"

namespace captived {

namespace {
bool is_valid_mode(std::string mode) {
  return (mode == MODE_PASSTHROUGH || mode == MODE_SECURE_HOST ||
          mode == MODE_SECURE_LAN);
}
} // namespace

namespace rest {

mode::mode(std::string path, system& system, std::string target_path,
           std::string mode_symlink)
    : resource(path), system_(system), mode_symlink_(mode_symlink),
      target_path_(target_path) {}

/*******************************************************************************
 * get
 * Handle the get operation on the /mode REST resource.
 ******************************************************************************/
mode::resp_type mode::get(resource::req_type) {
  auto temp_mode = router_mode();
  if (!temp_mode) {
    auto msg = "Error: failed to find router mode.";
    return internal_server_error(json::string(msg));
  }

  return ok(json::string(*temp_mode));
}

/*******************************************************************************
 * put
 * Handle the 'put' operation on the /mode REST resource
 ******************************************************************************/
mode::resp_type mode::put(resource::req_type body) {
  // Parse and validate the request body
  json_t* root = body.get();
  std::string old_mode_str;

  // get the mode before we change it.
  auto old_mode = router_mode();
  if (old_mode) {
    old_mode_str = *old_mode;
  }

  if (!json_is_string(root)) {
    auto msg = "Error: request body is not a JSON string.";
    return bad_request(json::string(msg));
  }

  std::string newval = json_string_value(root);

  if (!is_valid_mode(newval)) {
    auto msg = "Error: mode is invalid.";
    return bad_request(json::string(msg));
  }

  if (newval != old_mode_str) {
    // only need to update the resource if we have a new value
    if (!router_mode(newval)) {
      auto msg = "Error: failed to update resource.";
      return internal_server_error(json::string(msg));
    }
  }
  return get(json::null());
}

/*******************************************************************************
 * Returns the configured mode of the router card.
 * Will be one of "passthrough", "secure-host", or "secure-lan".
 *
 * @returns The router card mode or None on an error.
 ******************************************************************************/
std::experimental::optional<std::string> mode::router_mode() {
  auto fq_link = system_.symlink_target(mode_symlink_);
  if (!fq_link) {
    return {};
  }

  size_t period = fq_link->find_last_of('.');
  size_t slash = fq_link->find_last_of('/');
  std::string mode = fq_link->substr(slash + 1, period - slash - 1);
  return {mode};
}

/*******************************************************************************
 * Set the router card's configured mode.
 *
 * The change will be stored, but not take effect until a reboot.
 *
 * @param new_mode The mode to set. Valid input modes are: "passthrough",
 *     "secure-host", or "secure-lan".
 * @returns true on success, false otherwise.
 ******************************************************************************/
bool mode::router_mode(std::string new_mode) {
  std::string target_name = target_path_ + "/" + new_mode + ".target";
  return system_.symlink_target(target_name, mode_symlink_);
}

} // namespace rest
} // namespace captived
