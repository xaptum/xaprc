#include "rest/uptime.hpp"
#include "json.hpp"

#include <iostream>

namespace captived {
namespace rest {

uptime::uptime(std::string path, system system)
    : resource(path), system_(system) {}

std::experimental::optional<double> uptime::seconds() {
  auto line = system_.readline("/proc/uptime");
  if (!line)
    return {};

  // Read the first number from the line
  std::cout << *line << std::endl;
  double seconds;
  std::stringstream ss(*line);
  ss >> seconds;
  if (ss.fail())
    return {};

  return {seconds};
}

uptime::resp_type uptime::get(uptime::req_type) {
  auto seconds_opt = seconds();
  if (!seconds_opt) {
    auto msg = "Error: failed to read uptime";
    return internal_server_error(json::string(msg));
  }

  return ok(json::number(*seconds_opt));
}

} // namespace rest
} // namespace captived
