#pragma once

#include <experimental/optional>
#include <string>
#include <vector>

#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class wifi_config : public resource {
  public:
    /**
     * secure-host and secure-lan share the same config. The connman
     * interactions are simplest if each mode has its own config file,
     * but the API is simplest if just one config is exposed.
     *
     * This resource optimizes for both by storing the same config in
     * multiple files.  The first one in the vector is used for
     * GETs. All are updated on PUT/POSTs.
     */
    wifi_config(std::string path,
                system& system,
                std::vector<std::string> config_files);
    ~wifi_config() override = default;

    resp_type get(req_type) override;
    resp_type put(req_type body) override;

    /**
     * Fetch the wifi configuration contents.
     *
     * @returns The configuration contents or None on an error.
     */
    std::experimental::optional<std::string> contents();

    /**
     * Updates the wifi configuration contents.
     *
     * @returns true on success and false on an error
     */
    bool contents(std::string new_contents);

    /**
     * Fetch the SHA256 hash of the configuration contents.
     *
     * @returns The hash of the configuration contents or None on an error.
     */
    std::experimental::optional<std::string> sha256();

  protected:
    system& system_;
    std::vector<std::string> config_files_;
};

}    // namespace rest
}    // namespace captived
