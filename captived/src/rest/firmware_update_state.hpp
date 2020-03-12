#pragma once

#include <regex>
#include <string>

#include "firmware_manager.hpp"
#include "rest/resource.hpp"

namespace captived {
namespace rest {

class firmware_update_state : public resource {
  public:
    firmware_update_state(std::string path, firmware_manager& fw_mgr);
    ~firmware_update_state() override = default;

    resp_type get(req_type) override;

  protected:
    firmware_manager& fw_mgr_;
};

}    // namespace rest
}    // namespace captived
