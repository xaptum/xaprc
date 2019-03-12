#ifndef CAPTIVERC_DEFINES_H
#define CAPTIVERC_DEFINES_H

#include <string>

namespace captiverc {

const unsigned short s_http_port = 4000;

const std::string FILE_SERIAL_NUMBER = "/rom/serial";
const std::string FILE_WIFI_MAC_ADDRESS = "/rom/mac_address/1";
const std::string FILE_ENF_CONTROL_ADDRESS = "/data/enftun/enf1/address";
const std::string FILE_ENF_DATA_ADDRESS = "/data/enftun/enf0/address";
const std::string FILE_FIRMWARE_VERSION = "/etc/mender/artifact_info"; 
const std::string FILE_ROUTER_MODE = "/data/default_target";
const std::string FILE_WIFI_CONFIG = "/data/wpa_supplicant/wpa_supplicant-nl80211-wlan0.conf";
const std::string FILE_REBOOT_EXE = "/sbin/reboot";

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
const std::string URI_REBOOT = "/reboot";

// valid values for /mode URI
const std::string MODE_PASSTHROUGH = "passthrough";
const std::string MODE_SECURE_HOST = "secure_host";
const std::string MODE_SECURE_LAN = "secure_lan";

extern const char* CONTENT_TYPE_JSON;


}  // namespace captiverc
#endif