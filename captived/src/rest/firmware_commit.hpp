#pragma once

#include <string>

#include "firmware_manager.hpp"
#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class firmware_commit : public resource {
  public:
    firmware_commit(std::string path,
                    system& system,
                    firmware_manager& fw_mgr,
                    std::string commit_exe);

    resp_type put(req_type body) override;

    /**
     * Executes a firmware commit.
     *
     * @returns 0 on success and an error code otherwise.
     */
    int execute();

  protected:
    system& system_;
    std::string commit_exe_;
    firmware_manager& fw_mgr_;
};

}    // namespace rest
}    // namespace captived
