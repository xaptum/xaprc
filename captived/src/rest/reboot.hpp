#ifndef REBOOT_H
#define REBOOT_H

#include <string>

#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

class reboot : public resource {
  public:
    reboot(std::string path,
           std::string reboot_exe);

    resource::resp_type post(resource::req_type body) override;

  protected:
    std::string reboot_exe_;
};

} // namespace rest
} // namespace captiverc


#endif
