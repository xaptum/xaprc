#pragma once

#include <string>

#include "system.hpp"
#include "rest/resource.hpp"
#include "rest/line_resource.hpp"

namespace captiverc {
namespace rest {

/**
 * A resource for the router card mode.
 */
  class mode : public line_resource {
  public:
    mode(std::string path, system& system,
         std::string filename);

    bool line(std::string new_line) override;

    resp_type put(req_type body) override;
};

} // namespace rest
} // namespace captiverc
