#pragma once

#include <experimental/optional>
#include <string>

#include "system.hpp"
#include "rest/resource.hpp"

namespace captived {
namespace rest {

  class wifi_config : public resource {
  public:
    wifi_config(std::string path, system system,
                std::string config_file);
    ~wifi_config() override = default;

    resp_type get(req_type) override;
    resp_type put(req_type body) override;

    /**
     * Fetch the wifi configuration contents.
     *
     * @returns The configuration contents or None on an error.
     */
    std::experimental::optional<std::string>
    contents();

    /**
     * Updates the wifi configuration contents.
     *
     * @returns true on success and false on an error
     */
    bool
    contents(std::string new_contents);

    /**
     * Fetch the SHA256 hash of the configuration contents.
     *
     * @returns The hash of the configuration contents or None on an error.
     */
    std::experimental::optional<std::string>
    sha256();

  protected:
    system system_;
    std::string config_file_;
};

} // namespace rest
} // namespace captived
