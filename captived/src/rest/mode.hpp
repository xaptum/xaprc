#pragma once

#include <string>

#include "rest/line_resource.hpp"
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
       std::string mode_symlink);
  ~mode() override = default;

  resp_type get(req_type) override;
  resp_type put(req_type body) override;

  /**
   * Fetch the mode.
   *
   * @returns The mode or None on an error.
   */
  virtual std::experimental::optional<std::string> router_mode();

  /**
   * Set the mode.
   *
   * @returns true on success and false on an error
   */
  virtual bool router_mode(std::string new_mode);

protected:
  system& system_;
  std::string mode_symlink_; // fully qualified path to symlink we modify
  std::string target_path_;  // path to the systemd file where we point.
};

} // namespace rest
} // namespace captived
