#include "json.hpp"
#include "rest/aggregate_resource.hpp"

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

  aggregate_resource::aggregate_resource(std::string path) :
    resource(path)
  {}

  void
  aggregate_resource::add(std::string name, resource& res) {
    children_.push_back({name, {res}});
  }

  aggregate_resource::resp_type
  aggregate_resource::get(req_type) {
    auto root = json::object();

    for (auto child : children_) {
      auto name = std::get<0>(child);
      auto res = std::get<1>(child);
      add_resource(root, name, res);
    }

    return ok(root);
}

} // namespace rest
} // namespace captived
