#include "captive-server-libevent.h"
#include <fstream>
#include <iostream>
#include <string>
#include "defines.h"
#include "http_server.h"
#include "rest_resource_to_file.h"
#include "rest_resource_router_mode.h"

////////////////////////////////////////////////////////////////////////////////
//
// main()
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    std::cout << "About to run the web server." << std::endl;

    using namespace captiverc;
    http_server embed_server(4000);

    rest_resource_to_file ser_num_res = rest_resource_to_file (
                            URI_SERIAL_NUMBER, 
                            &embed_server,
                            FILE_SERIAL_NUMBER,
                            false
                            );

    rest_resource_to_file mac_addr_res = rest_resource_to_file (
                            URI_WIFI_MAC_ADDRESS,
                            &embed_server,
                            FILE_WIFI_MAC_ADDRESS,
                            false
                            );

    rest_resource_to_file control_addr_resource = rest_resource_to_file (
                            URI_ENF_CONTROL_ADDRESS,
                            &embed_server,
                            FILE_ENF_CONTROL_ADDRESS,
                            false
                            );

    rest_resource_to_file data_addr_resource = rest_resource_to_file (
                            URI_ENF_DATA_ADDRESS,
                            &embed_server,
                            FILE_ENF_DATA_ADDRESS,
                            false
                            );

    rest_resource_to_file firmware_ver_resource = rest_resource_to_file (
                            URI_FIRMWARE_VERSION,
                            &embed_server,
                            FILE_FIRMWARE_VERSION,
                            false
                            );

    rest_resource_router_mode router_mode_resource = rest_resource_router_mode (
                            URI_ROUTER_MODE,
                            &embed_server,
                            FILE_ROUTER_MODE
                            );



    embed_server.loop_dispatch();

    std::cout << "Finished running web server - exiting." << std::endl;

    return 0;
}
