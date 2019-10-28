#include "rest/model.hpp"
#include "json.hpp"

namespace captived {
namespace rest {

model::model(std::string path, system& system, std::string filename)
    : resource(path), system_(system), filename_(filename) {}

std::experimental::optional<std::string>
model::model_number() {
    auto line_opt = system_.readline(filename_);
    if (!line_opt)
        return {};

    std::regex re("artifact_name=([\\w-]+)-v\\d+\\.\\d+\\.\\d+-\\d+-g[0-9a-hA-H]+$");
    std::smatch match;
    std::regex_search(*line_opt, match, re);
    if (match.size() < 1)
        return {};

    return {match.str(1)};
}

model::resp_type
model::get(resource::req_type) {
    auto model_opt = model_number();
    if (!model_opt) {
        auto msg = "Error: failed to read resource";
        return internal_server_error(json::string(msg));
    }

    return ok(json::string(*model_opt));
}

}    // namespace rest
}    // namespace captived
