#pragma once

#include <string_view>
#include <string>
#include <expected>

namespace AbysslandSocket {
    std::expected<std::string, std::string> getFromSocket(const std::string& cmd);
};