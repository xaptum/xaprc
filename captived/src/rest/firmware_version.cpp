#include "rest/firmware_version.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

firmware_version::firmware_version(std::string path,
                                   system& system,
                                   std::string filename)
    : resource(path), system_(system), filename_(filename) {}

std::experimental::optional<std::string>
firmware_version::version() {
    auto line_opt = system_.readline(filename_);
    if (!line_opt)
        return {};

    std::regex re(
        "artifact_name=[\\w-_]+-v(\\d+\\.\\d+\\.\\d+-\\d+)-g[0-9a-hA-H]+$");
    std::smatch match;
    std::regex_search(*line_opt, match, re);
    if (match.size() < 1)
        return {};

    return {match.str(1)};
}

firmware_version::resp_type
firmware_version::get(resource::req_type) {
    auto version_opt = version();
    if (!version_opt) {
        auto msg = "Error: failed to read resource";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*version_opt));
}

}    // namespace rest
}    // namespace captived
