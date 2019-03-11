#include "main.h"
#include <fstream>
#include <iostream>
#include <string>
#include "defines.h"
#include "http_server.h"
#include "rest_get_file.h"
#include "rest_put_file.h"
#include "rest_resource_root.h"
#include "rest_mode_get_put.h"
#include "rest_wifi_config.h"

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
    http_server embed_server(4000);

    rest_get_file ser_num_res = rest_get_file (
                            URI_SERIAL_NUMBER, 
                            FILE_SERIAL_NUMBER
                            );
    embed_server.register_resource(ser_num_res);

    rest_get_file mac_addr_res = rest_get_file (
                            URI_WIFI_MAC_ADDRESS,
                            FILE_WIFI_MAC_ADDRESS
                            );
    embed_server.register_resource(mac_addr_res);

    rest_get_file control_addr_resource = rest_get_file (
                            URI_ENF_CONTROL_ADDRESS,
                            FILE_ENF_CONTROL_ADDRESS
                            );
    embed_server.register_resource(control_addr_resource);

    rest_get_file data_addr_resource = rest_get_file (
                            URI_ENF_DATA_ADDRESS,
                            FILE_ENF_DATA_ADDRESS
                            );
    embed_server.register_resource(data_addr_resource);

    rest_get_file firmware_ver_resource = rest_get_file (
                            URI_FIRMWARE_VERSION,
                            FILE_FIRMWARE_VERSION
                            );
    embed_server.register_resource(firmware_ver_resource);

    rest_mode_get_put router_mode_resource = rest_mode_get_put (
                            URI_ROUTER_MODE,
                            FILE_ROUTER_MODE
                            );
    embed_server.register_resource(router_mode_resource);


    rest_resource_root root_resource = rest_resource_root();
    embed_server.register_resource(root_resource);

    rest_wifi_config wifi_config_resource = rest_wifi_config (
                            URI_WIFI_CONFIG,
                            FILE_WIFI_CONFIG
                            );
    embed_server.register_resource(wifi_config_resource);


    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
