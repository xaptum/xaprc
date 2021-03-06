#include "picosha2.hpp"

#include "rest/resource.hpp"
#include "rest/wifi_config.hpp"

namespace {
std::string
sha256_hex(std::string str) {
    std::string hash;
    picosha2::hash256_hex_string(str, hash);
    return hash;
}
}    // namespace

namespace captived {
namespace rest {

wifi_config::wifi_config(std::string path,
                         system& system,
                         std::string config_file)
    : resource(path), config_file_(config_file), system_(system) {}

std::experimental::optional<std::string>
wifi_config::contents() {
    return system_.read(config_file_);
}

bool
wifi_config::contents(std::string new_contents) {
    // Don't need to update if the contents did not change.
    if (sha256_hex(new_contents) == *sha256()) {
        return true;
    }

    if (system_.write(config_file_, new_contents)) {
        int ret_code = system_.execute(COMMAND_RELOAD_CONNMAN);
        return true;
    }

    return false;
}

std::experimental::optional<std::string>
wifi_config::sha256() {
    auto contents_str = contents();
    if (!contents_str)
        return {};

    return {sha256_hex(*contents_str)};
}

wifi_config::resp_type
wifi_config::get(req_type) {
    auto contents_str = contents();
    if (!contents_str) {
        auto msg = "Error: failed to read config";
        return internal_server_error(json::string(msg));
    }

    auto root = json::object();
    json::object_set(root, "contents", json::string(*contents_str));
    json::object_set(root, "sha256", json::string(sha256_hex(*contents_str)));
    return ok(root);
}

resource::resp_type
wifi_config::put(resource::req_type body) {
    // Parse and validate the request body
    json_t* root = body.get();

    if (!json_is_object(root)) {
        auto msg = "Error: request body is not a JSON object.";
        return bad_request(json::string(msg));
    }

    auto contents_json = json_object_get(root, "contents");
    if (NULL == contents_json) {
        auto msg = "Error:  'contents' element is missing.";
        return bad_request(json::string(msg));
    }

    if (!json_is_string(contents_json)) {
        auto msg = "Error: 'contents' value is not a string.";
        return bad_request(json::string(msg));
    }

    std::string newval = json_string_value(contents_json);

    if (!contents(newval)) {
        auto msg = "Error: failed to update config";
        return internal_server_error(json::string(msg));
    }
    return get(json::null());
}

}    // namespace rest
}    // namespace captived
