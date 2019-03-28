#pragma once

#include <memory>
#include <sstream>
#include <string>

#include <jansson.h>

namespace captived {
namespace json {

using json_type = std::shared_ptr<json_t>;

using error_type = json_error_t;

static json_type
json(json_t* ref) {
    return {ref, &json_decref};
}

static json_type
boolean(bool val) {
    return json(json_boolean(val));
}

static json_type
null() {
    return json(json_null());
}

static json_type
string(const std::string str) {
    return json(json_string(str.c_str()));
}

static json_type
string(const std::stringstream& stream) {
    return string(stream.str());
}

static json_type
number(json_int_t val) {
    return json(json_integer(val));
}

static json_type
number(double val) {
    return json(json_real(val));
}

static json_type
array() {
    return json(json_array());
}

static json_type
object() {
    return json(json_object());
}

static int
object_set(json_type obj, std::string key, json_type val) {
    return json_object_set(obj.get(), key.c_str(), val.get());
}

static std::string
dumps(json_type json) {
    auto flags = JSON_ENCODE_ANY | JSON_INDENT(2);
    std::unique_ptr<const char> str(json_dumps(json.get(), flags));
    return std::string(str.get());
}

static json_type
loads(std::string str, error_type* error) {
    // Treat an empty string as json null to simplify handling of
    // empty request bodies.
    if (str == "")
        return null();

    auto flags = JSON_DECODE_ANY;
    return json(json_loads(str.c_str(), flags, error));
}

}    // namespace json
}    // namespace captived
