#pragma once

#include <string>

#include "rest/resource.hpp"

namespace captived {
namespace rest {

class firmware_uri : public resource {
  public:
    firmware_uri(std::string path);
    ~firmware_uri() override = default;

    resp_type get(req_type) override;

    void uri(std::string uri) { uri_ = uri; }
    std::string uri() { return uri_; }

    void clear_uri() { uri_.clear(); }

  private:
    std::string uri_;
};

}    // namespace rest
}    // namespace captived
