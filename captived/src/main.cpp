#include <event2/event.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

#include "defines.hpp"
#include "system.hpp"
#include "tasks.hpp"

#include "http/server.hpp"
#include "rest/aggregate_resource.hpp"
#include "rest/firmware.hpp"
#include "rest/firmware_commit.hpp"
#include "rest/firmware_image.hpp"
#include "rest/firmware_update_state.hpp"
#include "rest/firmware_version.hpp"
#include "rest/line_resource.hpp"
#include "rest/mode.hpp"
#include "rest/model.hpp"
#include "rest/reboot.hpp"
#include "rest/uptime.hpp"
#include "rest/wifi_config.hpp"
#include "rest/wifi_status.hpp"

namespace captived {
const char* CONTENT_TYPE_JSON = "application/json";
}

int
main(int argc, char* argv[]) {
    std::cout << "About to run the web server." << std::endl;

    using namespace captived;
    std::string root_path;    // start with a blank root path

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-p", 2) == 0) {
            root_path = argv[i + 1];
            std::cout << "\nSetting path to:" << root_path << std::endl;
        }
    }

    captived::system sys(root_path);

    captived::tasks init_tasks(sys);
    init_tasks.run_init_tasks();

    rest::line_resource serial_number(
        URI_SERIAL_NUMBER, sys, FILE_SERIAL_NUMBER, false);

    rest::line_resource mac_addr_1(
        URI_WIFI_MAC_ADDRESS_1, sys, FILE_WIFI_MAC_ADDRESS_1, false);

    rest::line_resource mac_addr_2(
        URI_WIFI_MAC_ADDRESS_2, sys, FILE_WIFI_MAC_ADDRESS_2, false);

    rest::line_resource mac_addr_3(
        URI_WIFI_MAC_ADDRESS_3, sys, FILE_WIFI_MAC_ADDRESS_3, false);

    rest::line_resource mac_addr_4(
        URI_WIFI_MAC_ADDRESS_4, sys, FILE_WIFI_MAC_ADDRESS_4, false);

    rest::aggregate_resource mac_addrs(URI_WIFI_MAC_ADDRESS);
    mac_addrs.add("1", mac_addr_1);
    mac_addrs.add("2", mac_addr_2);
    mac_addrs.add("3", mac_addr_3);
    mac_addrs.add("4", mac_addr_4);

    rest::line_resource control_addr(
        URI_ENF_CONTROL_ADDRESS, sys, FILE_ENF_CONTROL_ADDRESS, false);

    rest::line_resource data_addr(
        URI_ENF_DATA_ADDRESS, sys, FILE_ENF_DATA_ADDRESS, false);

    // Initializes the libevent event base
    // Needed for the http server and the event handling
    std::shared_ptr<event_base> base(event_base_new(), &event_base_free);

    // This handles the specifics of the firmware update mechanism (mender)
    firmware_manager fw_mgr(sys, base);

    rest::firmware_image firmware_image(
        URI_FIRMWARE_VERSION, sys, FILE_FIRMWARE_VERSION);

    rest::firmware_version firmware_running_ver(
        URI_FIRMWARE_RUNNING_VERSION, sys, FILE_FIRMWARE_VERSION);

    rest::firmware_update_state firmware_state(URI_FIRMWARE_UPDATE_STATE,
                                               fw_mgr);

    rest::firmware_uri firmware_uri(URI_FIRMWARE_UPDATE_URI);

    rest::firmware firmware(URI_FIRMWARE, sys, fw_mgr, firmware_uri);
    firmware.add("running_version", firmware_running_ver);
    firmware.add("running_image", firmware_image);
    firmware.add("update_state", firmware_state);

    rest::firmware_commit commit(
        URI_FIRMWARE_COMMIT, sys, fw_mgr, COMMAND_MENDER_COMMIT);

    rest::model model_number(URI_MODEL, sys, FILE_FIRMWARE_VERSION);

    rest::wifi_config wifi_config_passthrough(
        URI_WIFI_CONFIG_PASSTHROUGH, sys, FILE_WIFI_CONFIG_PASSTHROUGH);

    rest::wifi_config wifi_config_secure(
        URI_WIFI_CONFIG_SECURE, sys, FILE_WIFI_CONFIG_SECURE_HOST);

    rest::aggregate_resource wifi_configs(URI_WIFI_CONFIG);
    wifi_configs.add("passthrough", wifi_config_passthrough);
    wifi_configs.add("secure", wifi_config_secure);

    rest::wifi_status wifi_status(URI_WIFI_STATUS, sys);

    rest::aggregate_resource wifi(URI_WIFI);
    wifi.add("config", wifi_configs);
    wifi.add("status", wifi_status);

    rest::uptime uptime(URI_UPTIME, sys);

    rest::reboot reboot(URI_REBOOT, sys, FILE_REBOOT_EXE);

    rest::mode router_mode(
        URI_ROUTER_MODE, sys, PATH_MODE_TARGET, LINK_MODE, reboot);

    rest::aggregate_resource root(URI_ROUTER_STATUS);
    root.add("serial_number", serial_number);
    root.add("firmware_version", firmware_image);
    root.add("firmware", firmware);
    root.add("model", model_number);
    root.add("mac_address", mac_addrs);
    root.add("control_address", control_addr);
    root.add("data_address", data_addr);
    root.add("mode", router_mode);
    root.add("uptime", uptime);
    root.add("wifi", wifi);

    http::server embed_server(4000, root_path, base);

    embed_server.register_resource(serial_number);
    embed_server.register_resource(mac_addr_1);
    embed_server.register_resource(mac_addr_2);
    embed_server.register_resource(mac_addr_3);
    embed_server.register_resource(mac_addr_4);
    embed_server.register_resource(mac_addrs);
    embed_server.register_resource(control_addr);
    embed_server.register_resource(data_addr);
    embed_server.register_resource(firmware_image);
    embed_server.register_resource(firmware);
    embed_server.register_resource(commit);
    embed_server.register_resource(model_number);
    embed_server.register_resource(router_mode);
    embed_server.register_resource(uptime);
    embed_server.register_resource(wifi_config_passthrough);
    embed_server.register_resource(wifi_config_secure);
    embed_server.register_resource(wifi_configs);
    embed_server.register_resource(wifi_status);
    embed_server.register_resource(wifi);
    embed_server.register_resource(reboot);
    embed_server.register_resource(root);

    using atomic_bool = std::atomic<bool>;
    atomic_bool continue_running(true);

    // Callback function for signals that will exit the loop.
    auto op = [](evutil_socket_t fd, short what, void* arg) {
        atomic_bool* continue_running = static_cast<atomic_bool*>(arg);
        *continue_running = false;
        std::cout << "Exiting Web Server loop." << std::endl;
    };

    event_ptr sigint_event(event_ptr(
        evsignal_new(base.get(), SIGINT, op, base.get()), event_free));
    evsignal_add(sigint_event.get(), NULL);

    event_ptr sigterm_event(event_ptr(
        evsignal_new(base.get(), SIGTERM, op, base.get()), event_free));
    evsignal_add(sigterm_event.get(), NULL);

    // Runs the dispatch loop.
    struct timeval one_sec = {1, 0};
    while (continue_running) {
        event_base_loopexit(base.get(), &one_sec);
        event_base_dispatch(base.get());
    }

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
