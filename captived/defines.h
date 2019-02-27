#ifndef CAPTIVERC_DEFINES_H
#define CAPTIVERC_DEFINES_H

#include <string>

const unsigned short s_http_port = 4000;

#ifdef DEBUG
const std::string FILE_SERIAL_NUMBER = "config/serial_number";
const std::string FILE_WIFI_MAC_ADDRESS = "config/mac_address";
const std::string FILE_ENF_CONTROL_ADDRESS = "config/enf1/address";
const std::string FILE_ENF_DATA_ADDRESS = "config/enf0/address";
const std::string FILE_FIRMWARE_VERSION = "config/mender/artifact_info";
const std::string FILE_ROUTER_MODE = "config/router_mode";
#else
const std::string FILE_SERIAL_NUMBER = "/rom/serial";
const std::string FILE_WIFI_MAC_ADDRESS = "/rom/mac_address/1";
const std::string FILE_ENF_CONTROL_ADDRESS = "/data/enf1/address";
const std::string FILE_ENF_DATA_ADDRESS = "/data/enf0/address";
const std::string FILE_FIRMWARE_VERSION = "/etc/mender/artifact_info"; 
const std::string FILE_ROUTER_MODE = "/data/default_target";
#endif

const std::string URI_SERIAL_NUMBER = "/serial_number";
const std::string URI_WIFI_MAC_ADDRESS = "/mac_address";
const std::string URI_ENF_CONTROL_ADDRESS = "/control_address";
const std::string URI_ENF_DATA_ADDRESS = "/data_address";
const std::string URI_FIRMWARE_VERSION = "/firmware_version";
const std::string URI_ROUTER_MODE = "/mode";
const std::string URI_ROUTER_STATUS = "/";
const std::string URI_WIFI = "/wifi";
const std::string URI_WIFI_CONFIG = "/wifi/config";
const std::string URI_WIFI_STATUS = "/wifi/status";
const std::string URI_REBOOT_CARD = "/reboot";

// valid values for /mode URI
const std::string MODE_PASSTHROUGH = "passthrough";
const std::string MODE_SECURE_HOST = "secure_host";
const std::string MODE_SECURE_LAN = "secure_lan";



#endif