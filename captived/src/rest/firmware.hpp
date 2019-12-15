#pragma once

#include <functional>
#include <string>
#include <vector>

#include "firmware_manager.hpp"
#include "rest/aggregate_resource.hpp"
#include "rest/firmware_uri.hpp"

namespace captived {
namespace rest {

class firmware : public aggregate_resource {
  public:
    firmware(std::string path,
             system& system,
             firmware_manager& fw_mgr,
             firmware_uri& fw_uri);

    resp_type put(req_type body) override;

  protected:
    system& system_;
    firmware_manager& fw_mgr_;
    firmware_uri& fw_uri_;
};

}    // namespace rest
}    // namespace captived
