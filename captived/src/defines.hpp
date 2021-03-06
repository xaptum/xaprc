#pragma once

#include <event2/event.h>
#include <memory>
#include <string>

namespace captived {

const unsigned short s_http_port = 4000;

const std::string FILE_SERIAL_NUMBER = "/rom/serial";
const std::string FILE_WIFI_MAC_ADDRESS_1 = "/rom/mac_address/1";
const std::string FILE_WIFI_MAC_ADDRESS_2 = "/rom/mac_address/2";
const std::string FILE_WIFI_MAC_ADDRESS_3 = "/rom/mac_address/3";
const std::string FILE_WIFI_MAC_ADDRESS_4 = "/rom/mac_address/4";
const std::string FILE_ENF_CONTROL_ADDRESS = "/data/enftun/enf1/address";
const std::string FILE_ENF_DATA_ADDRESS = "/data/enftun/enf0/address";
const std::string FILE_FIRMWARE_VERSION = "/etc/mender/artifact_info";
const std::string FILE_WIFI_CONFIG_PASSTHROUGH =
    "/data/connman/passthrough/wifi.config";
const std::string FILE_WIFI_CONFIG_SECURE_HOST =
    "/data/connman/secure-host/wifi.config";
const std::string FILE_WIFI_CONFIG_SECURE_LAN =
    "/data/connman/secure-lan/wifi.config";

const std::string DIR_WIFI_CONFIG_SECURE_HOST = "/data/connman/secure-host";
const std::string DIR_WIFI_CONFIG_SECURE_LAN = "/data/connman/secure-lan";

const std::string FILE_REBOOT_EXE = "/sbin/reboot";
const std::string LINK_MODE = "/data/systemd/system/default.target";
const std::string PATH_MODE_TARGET = "/etc/systemd/system";

const std::string COMMAND_GET_ACTIVE_TARGETS =
    "/bin/systemctl list-units --type target";
const std::string COMMAND_RELOAD_CONNMAN =
    "/bin/systemctl kill -s HUP connman.service";
const std::string COMMAND_GET_SSID = "/sbin/iw dev wlan0 link";
const std::string COMMAND_FW_PRINTENV = "/sbin/fw_printenv";
const std::string COMMAND_MENDER_INSTALL = "/bin/mender -install";
const std::string COMMAND_MENDER_COMMIT = "/bin/mender -commit";

const std::string URI_SERIAL_NUMBER = "/serial_number";
const std::string URI_WIFI_MAC_ADDRESS = "/mac_address";
const std::string URI_WIFI_MAC_ADDRESS_1 = "/mac_address/1";
const std::string URI_WIFI_MAC_ADDRESS_2 = "/mac_address/2";
const std::string URI_WIFI_MAC_ADDRESS_3 = "/mac_address/3";
const std::string URI_WIFI_MAC_ADDRESS_4 = "/mac_address/4";
const std::string URI_ENF_CONTROL_ADDRESS = "/control_address";
const std::string URI_ENF_DATA_ADDRESS = "/data_address";
const std::string URI_FIRMWARE_VERSION = "/firmware_version";
const std::string URI_FIRMWARE = "/firmware";
const std::string URI_FIRMWARE_COMMIT = "/firmware/commit";
const std::string URI_FIRMWARE_UPDATE_STATE = "/firmware/update_state";
const std::string URI_FIRMWARE_UPDATE_URI = "/firmware/update_uri";
const std::string URI_FIRMWARE_RUNNING_VERSION = "/firmware/running_version";
const std::string URI_MODEL = "/model";
const std::string URI_ROUTER_MODE = "/mode";
const std::string URI_ROUTER_STATUS = "/";
const std::string URI_UPTIME = "/uptime";
const std::string URI_WIFI = "/wifi";
const std::string URI_WIFI_CONFIG = "/wifi/config";
const std::string URI_WIFI_CONFIG_PASSTHROUGH = "/wifi/config/passthrough";
const std::string URI_WIFI_CONFIG_SECURE = "/wifi/config/secure";
const std::string URI_WIFI_STATUS = "/wifi/status";
const std::string URI_REBOOT = "/reboot";

// valid values for /mode URI
const std::string MODE_PASSTHROUGH = "passthrough";
const std::string MODE_SECURE_HOST = "secure-host";
const std::string MODE_SECURE_LAN = "secure-lan";

// firmware environment variables
const std::string FIRMWARE_ENV_BOOT_COUNT = "bootcount";
const std::string FIRMWARE_ENV_UPGRADE_AVAIL = "upgrade_available";

// valid states in the firmware update cycle - FIRMWARE.STATE
const std::string FIRMWARE_STATE_NORMAL = "normal";
const std::string FIRMWARE_STATE_DOWNLOADING = "downloading";
const std::string FIRMWARE_STATE_DOWNLOADED = "downloaded";
const std::string FIRMWARE_STATE_UNCOMMITTED = "uncommitted";

extern const char* CONTENT_TYPE_JSON;

// define this so we don't use OS-specific values
const int ROUTER_CARD_PATH_MAX = 1024;

const std::string WIFI_INTERFACE_NAME = "wlan0";

using event_ptr = std::unique_ptr<struct event, decltype(&event_free)>;

}    // namespace captived
