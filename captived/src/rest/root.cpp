#include "json.hpp"
#include "rest/root.hpp"

namespace captived {
namespace rest {

namespace {

  /**
   * Adds the given resource as child element of the provided JSON
   * object under the specified key.
   */
  void add_resource(json::json_type& obj, std::string key, resource& res) {
    resource::resp_type resp;
    json::json_type val;

    resp = res.get(json::null());
    if (std::get<0>(resp) == http::status::ok)
      val = std::get<1>(resp);
    else
      val = json::null();

    json::object_set(obj, key, val);
  }

}

  root::root(std::string path,
             line_resource& serial_number,
             line_resource& firmware_version,
             line_resource& mac_addr,
             line_resource& control_addr,
             line_resource& data_addr,
             mode& mode) :
    resource(path),
    serial_number_(serial_number),
    firmware_version_(firmware_version),
    mac_addr_(mac_addr),
    control_addr_(control_addr),
    data_addr_(data_addr),
    mode_(mode)
  {}

  root::resp_type
  root::get(req_type) {
    auto root = json::object();

    add_resource(root, "serial_number", serial_number_);
    add_resource(root, "firmware_version", firmware_version_);
    add_resource(root, "mac_address", mac_addr_);
    add_resource(root, "control_address", control_addr_);
    add_resource(root, "data_address", data_addr_);
    add_resource(root, "mode", mode_);

    return ok(root);
}

} // namespace rest
} // namespace captived
