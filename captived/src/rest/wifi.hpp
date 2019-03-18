#pragma once

#include <string>

#include "rest/resource.hpp"
#include "rest/wifi_configs.hpp"

namespace captived {
namespace rest {

class wifi : public resource {
  public:
  wifi(std::string path,
       wifi_configs& configs);

  resp_type get(req_type) override;

  private:
  wifi_configs& configs_;

};

} // namespace rest
} // namespace captived
