#ifndef REBOOT_H
#define REBOOT_H

#include <string>

#include "http_server.h"
#include "resource.h"

namespace captiverc {

class rest_reboot : public resource {
  public:
    rest_reboot(std::string path,
                std::string reboot_exe);

    resource::resp_type post(resource::req_type body) override;

  protected:
    std::string reboot_exe_;
};

}   // namespace captiverc


#endif
