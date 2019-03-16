#ifndef REBOOT_H
#define REBOOT_H

#include <string>

#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

class reboot : public resource {
  public:
    reboot(std::string path, std::string reboot_exe);

    resp_type post(req_type body) override;

    /**
     * Schedules a reboot.
     *
     * @returns 0 on success and an error code otherwise. May not
     * return if the reboot process does not allow this process to
     * gracefully exit.
     */
    int execute();

  protected:
    std::string reboot_exe_;
};

} // namespace rest
} // namespace captiverc


#endif
