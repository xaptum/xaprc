#include "main.hpp"
#include <fstream>
#include <iostream>
#include <string>

#include "defines.hpp"
#include "system.hpp"

#include "http/server.hpp"
#include "rest/line_resource.hpp"
#include "rest/reboot.hpp"
#include "rest/root.hpp"
#include "rest/mode.hpp"
#include "rest/wifi_config.hpp"

namespace captiverc{
const char* CONTENT_TYPE_JSON = "application/json";
}

int main(int argc, char *argv[]) {
    std::cout << "About to run the web server." << std::endl;

    using namespace captiverc;
    std::string root_path;      // start with a blank root path

    for (int i = 1; i < argc; i++){
        if (strncmp(argv[i], "-p", 2) == 0) {
            root_path = argv[i+1];
            std::cout << "\nSetting path to:" << root_path << std::endl;
        }
    }

    captiverc::system sys(root_path);

    rest::line_resource serial_number(URI_SERIAL_NUMBER, sys,
                                      FILE_SERIAL_NUMBER, false);


    rest::line_resource mac_addr(URI_WIFI_MAC_ADDRESS, sys,
                                    FILE_WIFI_MAC_ADDRESS, false);

    rest::line_resource control_addr(URI_ENF_CONTROL_ADDRESS, sys,
                                     FILE_ENF_CONTROL_ADDRESS, false);


    rest::line_resource data_addr(URI_ENF_DATA_ADDRESS, sys,
                                  FILE_ENF_DATA_ADDRESS, false);

    rest::line_resource firmware_version(URI_FIRMWARE_VERSION, sys,
                                         FILE_FIRMWARE_VERSION, false);

    rest::mode router_mode(URI_ROUTER_MODE, sys,
                           FILE_ROUTER_MODE);

    rest::wifi_config wifi_config_passthrough(URI_WIFI_CONFIG_PASSTHROUGH, sys,
                                              FILE_WIFI_CONFIG_PASSTHROUGH);


    rest::wifi_config wifi_config_secure_host(URI_WIFI_CONFIG_SECURE_HOST, sys,
                                              FILE_WIFI_CONFIG_SECURE_HOST);

    rest::reboot reboot(URI_REBOOT, sys, FILE_REBOOT_EXE);

    rest::root root(root_path);

    http::server embed_server(4000, root_path);

    embed_server.register_resource(serial_number);
    embed_server.register_resource(mac_addr);
    embed_server.register_resource(control_addr);
    embed_server.register_resource(data_addr);
    embed_server.register_resource(firmware_version);
    embed_server.register_resource(router_mode);
    embed_server.register_resource(wifi_config_passthrough);
    embed_server.register_resource(wifi_config_secure_host);
    embed_server.register_resource(reboot);
    embed_server.register_resource(root);

    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
