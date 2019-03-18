#pragma once

#include <string>

#include "rest/resource.hpp"
#include "rest/wifi_config.hpp"

namespace captived {
namespace rest {

class wifi_configs : public resource {
  public:
  wifi_configs(std::string path,
               wifi_config& passthrough,
               wifi_config& secure_host);

  resp_type get(req_type) override;

  private:
  wifi_config& passthrough_;
  wifi_config& secure_host_;

};

} // namespace rest
} // namespace captived
