#include "rest/firmware.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

firmware::firmware(std::string path, system& system, std::string filename)
    : resource(path), system_(system), filename_(filename) {}

std::experimental::optional<std::string>
firmware::version() {
    auto line_opt = system_.readline(filename_);
    if (!line_opt)
        return {};

    std::regex re("artifact_name=(.*)");
    std::smatch match;
    std::regex_search(*line_opt, match, re);
    if (match.size() < 1)
        return {};

    return {match.str(1)};
}

firmware::resp_type
firmware::get(resource::req_type) {
    auto version_opt = version();
    if (!version_opt) {
        auto msg = "Error: failed to read resource";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*version_opt));
}

}    // namespace rest
}    // namespace captived
