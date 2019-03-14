#include "main.h"
#include <fstream>
#include <iostream>
#include <string>
#include "defines.h"
#include "http/http_server.h"
#include "rest/get_file.h"
#include "rest/put_file.h"
#include "rest/reboot.h"
#include "rest/root_resource.h"
#include "rest/mode_get_put.h"
#include "rest/wifi_config.h"

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

    http_server embed_server(4000, root_path);




    rest_get_file ser_num_res = rest_get_file (
                            URI_SERIAL_NUMBER, 
                            root_path + FILE_SERIAL_NUMBER
                            );
    embed_server.register_resource(ser_num_res);

    rest_get_file mac_addr_res = rest_get_file (
                            URI_WIFI_MAC_ADDRESS,
                            root_path + FILE_WIFI_MAC_ADDRESS
                            );
    embed_server.register_resource(mac_addr_res);

    rest_get_file control_addr_resource = rest_get_file (
                            URI_ENF_CONTROL_ADDRESS,
                            root_path + FILE_ENF_CONTROL_ADDRESS
                            );
    embed_server.register_resource(control_addr_resource);

    rest_get_file data_addr_resource = rest_get_file (
                            URI_ENF_DATA_ADDRESS,
                            root_path + FILE_ENF_DATA_ADDRESS
                            );
    embed_server.register_resource(data_addr_resource);

    rest_get_file firmware_ver_resource = rest_get_file (
                            URI_FIRMWARE_VERSION,
                            root_path + FILE_FIRMWARE_VERSION
                            );
    embed_server.register_resource(firmware_ver_resource);

    rest_mode_get_put router_mode_resource = rest_mode_get_put (
                            URI_ROUTER_MODE,
                            root_path + FILE_ROUTER_MODE
                            );
    embed_server.register_resource(router_mode_resource);


    rest_resource_root root_resource = rest_resource_root(root_path);
    embed_server.register_resource(root_resource);

    rest_wifi_config wifi_config_resource = rest_wifi_config (
                            URI_WIFI_CONFIG,
                            root_path + FILE_WIFI_CONFIG
                            );
    embed_server.register_resource(wifi_config_resource);

    rest_reboot reboot_res = rest_reboot (
                            URI_REBOOT, 
                            root_path + FILE_REBOOT_EXE
                            );
    embed_server.register_resource(reboot_res);

    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
