#pragma once

#include <string>

#include "rest/resource.hpp"
#include "rest/line_resource.hpp"
#include "rest/mode.hpp"
#include "rest/wifi.hpp"

namespace captived {
namespace rest {

class root : public resource {
  public:
  root(std::string path,
       line_resource& serial_number,
       line_resource& firmware_version,
       line_resource& mac_addr,
       line_resource& control_addr,
       line_resource& data_addr,
       mode& mode,
       wifi& wifi);

  resp_type get(req_type) override;

  private:
  line_resource& serial_number_;
  line_resource& firmware_version_;
  line_resource& mac_addr_;
  line_resource& control_addr_;
  line_resource& data_addr_;
  mode& mode_;
  wifi& wifi_;

};

} // namespace rest
} // namespace captived
