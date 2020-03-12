#include "rest/firmware_uri.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

firmware_uri::firmware_uri(std::string path) : resource(path), uri_() {}

firmware_uri::resp_type
firmware_uri::get(resource::req_type) {
    if (uri_.empty()) {
        return ok(json::null());
    }

    return ok(json::string(uri_));
}

}    // namespace rest
}    // namespace captived
