#pragma once

#include <json/json.h>

#include <memory>
#include <string>

inline std::string JsonToString(const Json::Value json) {
    return Json::writeString(Json::StreamWriterBuilder(), json);
}

inline Json::Value StringToJson(const std::string &json_str) {
    Json::Value json;
    std::string err;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.length(),
                       &json, &err)) {
        throw std::runtime_error("Failed to parse string to json, error: " +
                                 err);
    }
    return json;
}
