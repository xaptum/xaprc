#pragma once

#include <string>

#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class uptime : public resource {
  public:
    uptime(std::string path, system& system);
    ~uptime() override = default;

    resp_type get(req_type) override;

    /**
     * Returns the system uptime in seconds.
     */
    virtual std::experimental::optional<double> seconds();

  protected:
    system& system_;
};

}    // namespace rest
}    // namespace captived
