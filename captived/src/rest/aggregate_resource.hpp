#pragma once

#include <functional>
#include <string>
#include <vector>

#include "rest/resource.hpp"

namespace captived {
namespace rest {

class aggregate_resource : public resource {
  public:
    aggregate_resource(std::string path);

    void add(std::string name, resource& res, bool omit_null = false);

    resp_type get(req_type) override;

  private:
    std::vector<std::tuple<std::string, std::reference_wrapper<resource>, bool>>
        children_;
};

}    // namespace rest
}    // namespace captived
