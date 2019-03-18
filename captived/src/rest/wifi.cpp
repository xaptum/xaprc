#include "json.hpp"
#include "rest/wifi.hpp"

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

  wifi::wifi(std::string path,
             wifi_configs& configs) :
    resource(path),
    configs_(configs)
  {}

  wifi::resp_type
  wifi::get(req_type) {
    auto obj = json::object();

    add_resource(obj, "config", configs_);

    return ok(obj);
}

} // namespace rest
} // namespace captived
