#pragma once

#include <regex>
#include <string>

#include "rest/line_resource.hpp"
#include "rest/reboot.hpp"
#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

/**
 * A resource for the router card mode.
 */
class mode : public resource {
public:
  mode(std::string path, system& system, std::string target_path,
       std::string mode_symlink, reboot reboot_resource);
  ~mode() override = default;

  resp_type get(req_type) override;
  resp_type put(req_type body) override;

  /**
   * Set the mode and reboot.
   *
   * @returns true on success and false on an error
   */
  virtual bool router_mode(std::string new_mode);

  /**
   * Fetch the currently running mode.
   *
   * @returns The mode or None on an error.
   */
  virtual std::experimental::optional<std::string> router_mode();

private:
  /**
   * Fetch the configured mode. This may differ from the running mode if the
   * value was changed but, the router was not rebooted.
   *
   * @returns The mode or None on an error.
   */
  std::experimental::optional<std::string> router_mode_config();

  /**
   * Set (configure) the router mode, but do not reboot
   *
   * @returns true on success and false on an error
   */
  bool router_mode_config(std::string new_mode);

protected:
  system& system_;
  std::string mode_symlink_; // fully qualified path to symlink we modify
  std::string target_path_;  // path to the systemd file where we point.
  reboot reboot_resource_;   // Used to reboot system after mode change.
};

} // namespace rest
} // namespace captived
