#include "json.hpp"
#include "rest/line_resource.hpp"

namespace captived {
namespace rest {

    line_resource::line_resource(std::string path, system system,
                                 std::string filename, bool writable):
        resource(path),
        system_(system),
        filename_(filename),
        writable_(writable)
    {}

    std::experimental::optional<std::string>
    line_resource::line() {
        return system_.readline(filename_);
    }

    bool
    line_resource::line(std::string new_line) {
        if (!writable_)
            return false;

        return system_.writeline(filename_, new_line);
    }

    line_resource::resp_type
    line_resource::get(resource::req_type) {
        auto line_opt = line();
        if (!line_opt) {
            auto msg = "Error: failed to read resource";
            return internal_server_error(json::string(msg));
        }

        return ok(json::string(*line_opt));
    }

    line_resource::resp_type
    line_resource::put(resource::req_type body) {
        if (!writable_)
            return resource::put(body); // not allowed

        // Parse and validate the request body
        json_t* root = body.get();

        if (!json_is_string(root)) {
            auto msg = "Error: request body is not a JSON string.";
            return bad_request(json::string(msg));
        }

        std::string newval = json_string_value(root);

        // Update the resource
        if (!line(newval)) {
            auto msg = "Error: failed to update resource.";
            return internal_server_error(json::string(msg));
        }

        return get(json::null());
    }

} // namespace rest
} // namespace captived
