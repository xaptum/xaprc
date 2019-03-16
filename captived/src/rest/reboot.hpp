#ifndef REBOOT_H
#define REBOOT_H

#include <string>

#include "system.hpp"
#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

class reboot : public resource {
  public:
    reboot(std::string path, system system, std::string reboot_exe);

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
    system system_;
    std::string reboot_exe_;
};

} // namespace rest
} // namespace captiverc


#endif
