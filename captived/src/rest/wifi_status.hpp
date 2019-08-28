#pragma once

#include <arpa/inet.h>
#include <experimental/optional>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <regex>
#include <string>
#include <sys/types.h>

#include "jansson.h"
#include "json.hpp"
#include "rest/resource.hpp"
#include "system.hpp"

namespace captived {
namespace rest {

class wifi_status : public resource {
  public:
    wifi_status(std::string path, system& system);

    resp_type get(req_type) override;

  protected:
    system& system_;
    std::vector<std::string> ipv4_addrs_;
    std::vector<std::string> ipv6_addrs_;
    std::experimental::optional<std::string> ssid_;
    bool connected_;

    void refresh_connection_status();
    void refresh_ssid();

  private:
    void reset_connection() {
        ipv4_addrs_.clear();
        ipv6_addrs_.clear();
        ssid_ = std::experimental::nullopt;
        connected_ = false;
    }
};

}    // namespace rest
}    // namespace captived
