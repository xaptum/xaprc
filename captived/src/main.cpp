#include <fstream>
#include <iostream>
#include <string>

#include "defines.hpp"
#include "system.hpp"

#include "http/server.hpp"
#include "rest/aggregate_resource.hpp"
#include "rest/line_resource.hpp"
#include "rest/mode.hpp"
#include "rest/reboot.hpp"
#include "rest/uptime.hpp"
#include "rest/wifi_config.hpp"

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

    rest::line_resource firmware_version(
        URI_FIRMWARE_VERSION, sys, FILE_FIRMWARE_VERSION, false);

    rest::wifi_config wifi_config_passthrough(
        URI_WIFI_CONFIG_PASSTHROUGH, sys, FILE_WIFI_CONFIG_PASSTHROUGH);

    rest::wifi_config wifi_config_secure_host(
        URI_WIFI_CONFIG_SECURE_HOST, sys, FILE_WIFI_CONFIG_SECURE_HOST);

    rest::aggregate_resource wifi_configs(URI_WIFI_CONFIG);
    wifi_configs.add("passthrough", wifi_config_passthrough);
    wifi_configs.add("secure-host", wifi_config_secure_host);

    rest::aggregate_resource wifi(URI_WIFI);
    wifi.add("config", wifi_configs);

    rest::uptime uptime(URI_UPTIME, sys);

    rest::reboot reboot(URI_REBOOT, sys, FILE_REBOOT_EXE);

    rest::mode router_mode(
        URI_ROUTER_MODE, sys, PATH_MODE_TARGET, LINK_MODE, reboot);

    rest::aggregate_resource root("/");
    root.add("serial_number", serial_number);
    root.add("firmware_version", firmware_version);
    root.add("mac_address", mac_addrs);
    root.add("control_address", control_addr);
    root.add("data_address", data_addr);
    root.add("mode", router_mode);
    root.add("uptime", uptime);
    root.add("wifi", wifi);

    http::server embed_server(4000, root_path);

    embed_server.register_resource(serial_number);
    embed_server.register_resource(mac_addr_1);
    embed_server.register_resource(mac_addr_2);
    embed_server.register_resource(mac_addr_3);
    embed_server.register_resource(mac_addr_4);
    embed_server.register_resource(mac_addrs);
    embed_server.register_resource(control_addr);
    embed_server.register_resource(data_addr);
    embed_server.register_resource(firmware_version);
    embed_server.register_resource(router_mode);
    embed_server.register_resource(uptime);
    embed_server.register_resource(wifi_config_passthrough);
    embed_server.register_resource(wifi_config_secure_host);
    embed_server.register_resource(wifi_configs);
    embed_server.register_resource(wifi);
    embed_server.register_resource(reboot);
    embed_server.register_resource(root);

    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
