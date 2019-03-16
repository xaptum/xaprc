#include "rest/mode.hpp"

#include "defines.hpp"

namespace captiverc {

namespace {
    bool is_valid_mode(std::string mode) {
        return (mode == MODE_PASSTHROUGH ||
                mode == MODE_SECURE_HOST ||
                mode == MODE_SECURE_LAN);
    }
}

namespace rest {

    mode::mode(std::string path, system& system, std::string filename) :
        line_resource(path, system, filename, true)
    {}

    bool mode::line(std::string new_line) {
        return is_valid_mode(new_line) && line_resource::line(new_line);
    }

    mode::resp_type
    mode::put(mode::req_type body) {
        // Parse and validate the request body
        json_t* root = body.get();

        if (!json_is_string(root)) {
            auto msg = "Error: request body is not a JSON string.";
            return bad_request(json::string(msg));
        }

        std::string newval = json_string_value(root);

        if (!is_valid_mode(newval)) {
            auto msg = "Error: mode must is invalid.";
            return bad_request(json::string(msg));
        }

        return line_resource::put(body);
    }

} // namespace rest
} // namespace captiverc
