

#include "rest/wifi_status.hpp"
#include "rest/resource.hpp"

namespace captived {
namespace rest {

wifi_status::wifi_status(std::string path, system& system)
    : resource(path),
      system_(system),
      ipv4_addrs_(),
      ipv6_addrs_(),
      connected_(false) {}

wifi_status::resp_type
wifi_status::get(req_type) {
    refresh_connection_status();
    refresh_ssid();

    auto root = json::object();
    json::object_set(root, "connected", json::boolean(connected_));

    if (!ipv4_addrs_.empty()) {
        auto v4_addrs = json::array();
        for (auto addr : ipv4_addrs_) {
            json::array_append(v4_addrs, json::string(addr));
        }

        json::object_set(root, "IPv4_addresses", v4_addrs);
    }

    if (!ipv6_addrs_.empty()) {
        auto v6_addrs = json::array();
        for (auto addr : ipv6_addrs_) {
            json::array_append(v6_addrs, json::string(addr));
        }

        json::object_set(root, "IPv6_addresses", v6_addrs);
    }

    if (ssid_) {
        json::object_set(root, "SSID", json::string(*ssid_));
    }

    return ok(root);
}

/**
 * Reads connection status and stores relevant info in member variables
 *
 * @returns void
 */
void
wifi_status::refresh_connection_status() {
    reset_connection();

    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* temp_addrP = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (nullptr == ifa->ifa_addr) {
            continue;
        }

        std::string if_name(ifa->ifa_name);
        if (if_name != WIFI_INTERFACE_NAME) {
            continue;
        }

        // check for IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            temp_addrP = &(reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr))
                              ->sin_addr;
            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, temp_addrP, addr_buf, INET_ADDRSTRLEN);
            ipv4_addrs_.push_back(std::string(addr_buf));
            connected_ = connected_ || ((ifa->ifa_flags & IFF_RUNNING) != 0);
        }

        // check for IPv6
        if (ifa->ifa_addr->sa_family == AF_INET6) {
            temp_addrP =
                &(reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr))
                     ->sin6_addr;
            char addr_buf[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, temp_addrP, addr_buf, INET6_ADDRSTRLEN);
            ipv6_addrs_.push_back(std::string(addr_buf));
            connected_ = connected_ || ((ifa->ifa_flags & IFF_RUNNING) != 0);
        }
    }

    freeifaddrs(ifAddrStruct);
}

void
wifi_status::refresh_ssid() {
    std::string result;
    int ret_code = system_.execute(COMMAND_GET_SSID, result);
    if (ret_code != 0) {
        ssid_ = std::experimental::nullopt;
        return;
    }

    std::regex re("SSID: (.*)");

    std::smatch match;
    std::regex_search(result, match, re);
    if (match.size() < 1) {
        ssid_ = std::experimental::nullopt;
        return;
    }

    ssid_ = match.str(1);
    return;
}

}    // namespace rest
}    // namespace captived
