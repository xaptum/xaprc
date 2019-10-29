#pragma once

#include <regex>
#include <string>

#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class model : public resource {
  public:
    model(std::string path, system& system, std::string filename);
    ~model() override = default;

    resp_type get(req_type) override;

    /**
     * Return the model number.
     *
     * @returns The model number or None on an error.
     */
    virtual std::experimental::optional<std::string> model_number();

  protected:
    system& system_;
    std::string filename_;
};

}    // namespace rest
}    // namespace captived
