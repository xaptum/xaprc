#pragma once

#include <string>

#include "defines.hpp"
#include "system.hpp"

namespace captived {

class tasks {
  public:
    tasks(system& system) : system_(system){};

    /**
     * Initialization tasks needed before web server can run.
     *
     * Create the secure-lan directory as symbolic link to secure-host dir.
     */
    bool ensure_secure_lan_symlink_exists() {
        bool res = system_.create_symlink_to_config_dir(
            DIR_WIFI_CONFIG_SECURE_HOST, DIR_WIFI_CONFIG_SECURE_LAN);
        if (!res) {
            std::cerr << "ERROR: unable to create symbolic link "
                      << DIR_WIFI_CONFIG_SECURE_LAN << " to "
                      << DIR_WIFI_CONFIG_SECURE_HOST << std::endl;
        }

        return res;
    }

    bool run_init_tasks() { return ensure_secure_lan_symlink_exists(); }

  protected:
    system& system_;
};
}    // namespace captived