#include "main.hpp"
#include <fstream>
#include <iostream>
#include <string>

#include "defines.hpp"
#include "system.hpp"

#include "http/server.hpp"
#include "rest/get_file.hpp"
#include "rest/put_file.hpp"
#include "rest/reboot.hpp"
#include "rest/root.hpp"
#include "rest/mode_get_put.hpp"
#include "rest/wifi_config.hpp"

namespace captiverc{
const char* CONTENT_TYPE_JSON = "application/json";
}

////////////////////////////////////////////////////////////////////////////////
//
// main()
//
////////////////////////////////////////////////////////////////////////////////
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

    http::server embed_server(4000, root_path);


    rest::get_file ser_num_res = rest::get_file (
                            URI_SERIAL_NUMBER, 
                            root_path + FILE_SERIAL_NUMBER
                            );
    embed_server.register_resource(ser_num_res);

    rest::get_file mac_addr_res = rest::get_file (
                            URI_WIFI_MAC_ADDRESS,
                            root_path + FILE_WIFI_MAC_ADDRESS
                            );
    embed_server.register_resource(mac_addr_res);

    rest::get_file control_addr_resource = rest::get_file (
                            URI_ENF_CONTROL_ADDRESS,
                            root_path + FILE_ENF_CONTROL_ADDRESS
                            );
    embed_server.register_resource(control_addr_resource);

    rest::get_file data_addr_resource = rest::get_file (
                            URI_ENF_DATA_ADDRESS,
                            root_path + FILE_ENF_DATA_ADDRESS
                            );
    embed_server.register_resource(data_addr_resource);

    rest::get_file firmware_ver_resource = rest::get_file (
                            URI_FIRMWARE_VERSION,
                            root_path + FILE_FIRMWARE_VERSION
                            );
    embed_server.register_resource(firmware_ver_resource);

    rest::mode_get_put router_mode_resource = rest::mode_get_put (
                            URI_ROUTER_MODE,
                            root_path + FILE_ROUTER_MODE
                            );
    embed_server.register_resource(router_mode_resource);


    rest::root root = rest::root(root_path);
    embed_server.register_resource(root);

    rest::wifi_config wifi_config_passthrough_resource = rest::wifi_config (
                            URI_WIFI_CONFIG_PASSTHROUGH,
                            sys,
                            FILE_WIFI_CONFIG_PASSTHROUGH
                            );
    embed_server.register_resource(wifi_config_passthrough_resource);

    rest::wifi_config wifi_config_secure_host_resource = rest::wifi_config (
                            URI_WIFI_CONFIG_SECURE_HOST,
                            sys,
                            FILE_WIFI_CONFIG_SECURE_HOST
                            );
    embed_server.register_resource(wifi_config_secure_host_resource);

    rest::reboot reboot_res = rest::reboot (
                            URI_REBOOT,
                            sys,
                            FILE_REBOOT_EXE
                            );
    embed_server.register_resource(reboot_res);

    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
