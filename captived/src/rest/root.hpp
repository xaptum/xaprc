#pragma once

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "rest/resource.hpp"

namespace captived {
namespace rest {

class root : public resource {
  public:
    root(std::string root_path);

    resource::resp_type get(resource::req_type body) override;

  private:
    std::string get_file_contents(std::string filename);
    std::string get_status();

  private:
    std::string root_path_;

};

} // namespace rest
} // namespace captived
