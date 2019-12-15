#pragma once

#include <regex>
#include <string>

#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class firmware_version : public resource {
  public:
    firmware_version(std::string path, system& system, std::string filename);
    ~firmware_version() override = default;

    resp_type get(req_type) override;

    /**
     * Return the firmware version.
     *
     * @returns The version or None on an error.
     */
    virtual std::experimental::optional<std::string> version();

  protected:
    system& system_;
    std::string filename_;
};

}    // namespace rest
}    // namespace captived
