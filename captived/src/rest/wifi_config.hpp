#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <experimental/optional>
#include <string>

#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

  class wifi_config : public resource {
  public:
    wifi_config(std::string path, std::string config_file);
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
     * @returns The updated configuration contents or None on an error.
     */
    std::experimental::optional<std::string>
    contents(std::string new_contents);

    /**
     * Fetch the SHA256 hash of the configuration contents.
     *
     * @returns The hash of the configuration contents or None on an error.
     */
    std::experimental::optional<std::string>
    sha256();

  protected:
    std::string config_file_;

};

} // namespace rest
} // namespace captiverc


#endif
