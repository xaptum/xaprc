
#include <signal.h>
#include <sstream>
#include <sys/wait.h>

#include "defines.hpp"
#include "firmware_manager.hpp"

namespace captived {

/******************************************************************************
 * constructor
 ******************************************************************************/

firmware_manager::firmware_manager(system& system,
                                   std::shared_ptr<event_base> base)
    : system_(system),
      downloading_(false),
      base_(base),
      child_event_(nullptr, event_free),
      child_pid_(-1){

      };

/******************************************************************************
 * get_fw_env
 ******************************************************************************/
std::experimental::optional<std::string>
firmware_manager::get_fw_env(std::string env_var) {
    std::string env_string;
    std::string command = COMMAND_FW_PRINTENV + " " + env_var;
    int ret_code = system_.execute(command, env_string);
    if (ret_code != 0) {
        return {};
    }

    std::string re_pattern(env_var + "=(.+)");
    std::regex re(re_pattern);
    std::smatch match;
    std::regex_search(env_string, match, re);
    if (match.size() < 1) {
        return {};
    }

    auto match_val = match.str(1);
    return {match_val};
}

/******************************************************************************
 * firmware_state
 ******************************************************************************/
std::experimental::optional<std::string>
firmware_manager::firmware_state() {
    auto bootcount = get_fw_env(FIRMWARE_ENV_BOOT_COUNT);
    auto upgrade_avail = get_fw_env(FIRMWARE_ENV_UPGRADE_AVAIL);

    if (!bootcount || !upgrade_avail) {
        return {};
    }

    int bootcount_int = std::stoi(*bootcount);
    bool upgrade_avail_bool = (*upgrade_avail == "1");

    if (upgrade_avail_bool) {
        downloading_ = false;
        if (bootcount_int == 0) {
            return {FIRMWARE_STATE_DOWNLOADED};
        } else {
            return {FIRMWARE_STATE_UNCOMMITTED};
        }
    }

    if (downloading_) {
        return {FIRMWARE_STATE_DOWNLOADING};
    }

    return {FIRMWARE_STATE_NORMAL};
}

/******************************************************************************
 * download
 ******************************************************************************/
int
firmware_manager::download(std::string uri) {
    pid_t id = fork();

    if (id < 0) {
        return id;
    }

    // child process
    if (0 == id) {
        std::string command = COMMAND_MENDER_INSTALL + " " + uri;
        std::string mender_resp;
        int ret_code = system_.execute(command, mender_resp);
        exit(ret_code);
    }

    // Parent processing continues
    set_downloading();
    child_pid_ = id;

    auto op = [](evutil_socket_t fd, short what, void* arg) {
        firmware_manager* fw_mgr = static_cast<firmware_manager*>(arg);

        siginfo_t info;
        info.si_pid = 0;
        int wait_result =
            waitid(P_PID, fw_mgr->get_child_pid(), &info, WNOHANG | WEXITED);

        if (wait_result < 0) {
            std::stringstream msg;
            msg << "ERROR - error while waiting for child process "
                << fw_mgr->get_child_pid() << " to complete. " << std::endl
                << "\terror code:" << errno << std::endl;
            std::cout << msg.str();
            fw_mgr->set_debug_message(msg.str());
            return;
        }

        // return if child process sent a signal for other than exit.
        if (0 == info.si_pid) {
            return;
        }

        fw_mgr->clear_downloading();
        fw_mgr->delete_child_event();

        std::stringstream msg;
        if (CLD_KILLED == info.si_code) {
            msg << "ERROR - mender process was killed." << std::endl;
        } else if (CLD_EXITED == info.si_code) {
            msg << "mender exited with code: " << info.si_status << std::endl;
        } else {
            msg << "ERROR - mender process encountered unexpected event."
                << std::endl;
        }

        std::cout << msg.str();
        fw_mgr->set_debug_message(msg.str());
        return;
    };

    child_event_ =
        event_ptr(evsignal_new(base_.get(), SIGCHLD, op, this), event_free);
    if (0 != event_add(child_event_.get(), NULL)) {
        return -1;
    }

    return id;
}

}    // namespace captived
