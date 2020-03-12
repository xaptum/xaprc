#pragma once

#include <event2/event.h>
#include <experimental/optional>
#include <regex>
#include <string>

#include "system.hpp"
namespace captived {

/**
 * firmware_manager contains the logic to handle the firmware updates
 * This version uses mender.
 */
class firmware_manager {
  public:
    /**
     * Constructor
     *
     */
    firmware_manager(system& system, std::shared_ptr<event_base> base);

    // Don't need a copy constructor
    firmware_manager(const firmware_manager&) = delete;

    firmware_manager(firmware_manager&&) = default;

    /**
     * Returns the update state of the firmware.
     * Possible states are:
     *      normal
     *      downloading
     *      downloaded
     *      uncommitted
     *
     * @returns The state of a firmware update or an error
     */
    std::experimental::optional<std::string> firmware_state();

    /**
     * Indicate that downloading has finished
     *
     * @returns nothing
     */
    void clear_downloading() { downloading_ = false; }

    /**
     * Initiates the firmware download
     *
     * @returns PID of the child process. Negative value indicates an error.
     */
    int download(std::string uri);

    /**
     * sets the debug_msg_ message
     *
     * @returns nothing
     */
    void set_debug_message(std::string msg) { debug_msg_ = msg; }

    /**
     * Deletes the child event
     * event_free also makes the event non-pending.
     *
     * @returns nothing
     */
    void delete_child_event() { child_event_.reset(nullptr); }

    /**
     * Returns the child process id
     *
     * @returns child_pid_
     */
    int get_child_pid() { return child_pid_; }

  private:
    /**
     * Indicate that the download was started
     *
     * @returns nothing
     */
    void set_downloading() { downloading_ = true; }

    /**
     * get_fw_env
     *
     * Retrieves a requested firmware environment variable
     *
     * @param env_var The variable to return
     * @returns The value of the firmware-environment variable or {}
     */
    std::experimental::optional<std::string> get_fw_env(std::string);

    std::shared_ptr<event_base> base_;
    std::string debug_msg_;

    event_ptr child_event_;
    int child_pid_;

  protected:
    system& system_;
    bool downloading_;
};

}    // namespace captived
