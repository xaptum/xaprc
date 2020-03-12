#include "rest/aggregate_resource.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

namespace {

/**
 * Adds the given resource as child element of the provided JSON
 * object under the specified key.
 *
 * Doesn't add if value json_null and omit_null is true.
 */
void
add_resource(json::json_type& obj,
             std::string key,
             resource& res,
             bool omit_null) {
    resource::resp_type resp;
    json::json_type val;

    resp = res.get(json::null());
    if (std::get<0>(resp) == http::status::ok)
        val = std::get<1>(resp);
    else
        val = json::null();

    if (omit_null && json_is_null(val)) {
        return;
    }

    json::object_set(obj, key, val);
}

}    // namespace

aggregate_resource::aggregate_resource(std::string path) : resource(path) {}

/******************************************************************************
 * add
 ******************************************************************************/
void
aggregate_resource::add(std::string name, resource& res, bool omit_null) {
    children_.push_back({name, {res}, omit_null});
}

/******************************************************************************
 * get
 ******************************************************************************/
aggregate_resource::resp_type
aggregate_resource::get(req_type) {
    auto root = json::object();

    for (auto child : children_) {
        auto name = std::get<0>(child);
        auto res = std::get<1>(child);
        auto omit = std::get<2>(child);

        add_resource(root, name, res, omit);
    }

    return ok(root);
}

}    // namespace rest
}    // namespace captived
