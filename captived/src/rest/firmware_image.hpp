#pragma once

#include <regex>
#include <string>

#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class firmware_image : public resource {
  public:
    firmware_image(std::string path, system& system, std::string filename);
    ~firmware_image() override = default;

    resp_type get(req_type) override;

    /**
     * Return the firmware image name from the mender artifact file.
     * The image name will be in the format:
     *          xaprw001_dev-v1.0.13-16-g28c001d
     *
     * @returns The image name or None on an error.
     */
    virtual std::experimental::optional<std::string> image_name();

  protected:
    system& system_;
    std::string filename_;
};

}    // namespace rest
}    // namespace captived
