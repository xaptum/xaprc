#include "rest/firmware_image.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

firmware_image::firmware_image(std::string path,
                               system& system,
                               std::string filename)
    : resource(path), system_(system), filename_(filename) {}

std::experimental::optional<std::string>
firmware_image::image_name() {
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

firmware_image::resp_type
firmware_image::get(resource::req_type) {
    auto image_nm = image_name();
    if (!image_nm) {
        auto msg = "Error: failed to read resource";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*image_nm));
}

}    // namespace rest
}    // namespace captived
