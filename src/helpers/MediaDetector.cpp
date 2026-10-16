#include "MediaDetector.hpp"
#include "Logger.hpp"
#include "../defines.hpp"

#include <magic.h>
#include <cstring>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>

namespace MediaDetector {

namespace {
// PNG stores animation control before IDAT; ordinary PNGs stay on the image path.
bool isAnimatedPNG(const std::string_view path) {
    std::ifstream file(std::string{path}, std::ios::binary);
    std::array<unsigned char, 8> header;
    constexpr std::array<unsigned char, 8> signature{137, 80, 78, 71, 13, 10, 26, 10};
    if (!file.read(reinterpret_cast<char*>(header.data()), header.size()) || header != signature)
        return false;

    while (file.read(reinterpret_cast<char*>(header.data()), header.size())) {
        const uint32_t length = (uint32_t{header[0]} << 24) | (uint32_t{header[1]} << 16) |
                                (uint32_t{header[2]} << 8) | header[3];
        if (std::memcmp(header.data() + 4, "acTL", 4) == 0)
            return length == 8;
        if (std::memcmp(header.data() + 4, "IDAT", 4) == 0 || std::memcmp(header.data() + 4, "IEND", 4) == 0)
            return false;
        file.seekg(static_cast<std::streamoff>(length) + 4, std::ios::cur); // payload + CRC
    }
    return false;
}

bool isAnimatedWebP(const std::string_view path) {
    std::ifstream file(std::string{path}, std::ios::binary);
    std::array<unsigned char, 21> header;
    // An animated WebP requires the animation flag in its first VP8X chunk.
    return file.read(reinterpret_cast<char*>(header.data()), header.size()) &&
           std::memcmp(header.data(), "RIFF", 4) == 0 &&
           std::memcmp(header.data() + 8, "WEBPVP8X", 8) == 0 &&
           header[16] == 10 && header[17] == 0 && header[18] == 0 && header[19] == 0 &&
           (header[20] & 0x02);
}
}

eMediaType detectMediaType(const std::string_view& path) {
    magic_t magic = magic_open(MAGIC_MIME_TYPE);
    if (!magic) {
        g_logger->log(LOG_ERR, "Failed to initialize libmagic");
        return MEDIA_TYPE_UNKNOWN;
    }

    if (magic_load(magic, nullptr) != 0) {
        g_logger->log(LOG_ERR, "Failed to load magic database: {}", magic_error(magic));
        magic_close(magic);
        return MEDIA_TYPE_UNKNOWN;
    }

    const char* mimeType = magic_file(magic, std::string(path).c_str());
    if (!mimeType) {
        g_logger->log(LOG_ERR, "Failed to detect MIME type for {}: {}", path, magic_error(magic));
        magic_close(magic);
        return MEDIA_TYPE_UNKNOWN;
    }

    std::string mime(mimeType);
    magic_close(magic);

    g_logger->log(LOG_TRACE, "Detected MIME type for {}: {}", path, mime);

    // Check for video types
    if (mime.starts_with("video/")) {
        return MEDIA_TYPE_VIDEO;
    }

    // Check for image types that might be animated
    if (mime == "image/gif") {
        return MEDIA_TYPE_GIF;
    }
    
    if (mime == "image/png" || mime == "image/apng") {
        return isAnimatedPNG(path) ? MEDIA_TYPE_APNG : MEDIA_TYPE_IMAGE;
    }

    // Check for WebP (can be animated)
    if (mime == "image/webp") {
        return isAnimatedWebP(path) ? MEDIA_TYPE_VIDEO : MEDIA_TYPE_IMAGE;
    }

    // Static images
    if (mime.starts_with("image/")) {
        return MEDIA_TYPE_IMAGE;
    }

    // Additional video container checks by extension if MIME detection failed
    std::string pathStr(path);
    std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), [](unsigned char c) { return std::tolower(c); });
    
    if (pathStr.ends_with(".mp4") || pathStr.ends_with(".mkv") || 
        pathStr.ends_with(".webm") || pathStr.ends_with(".avi") ||
        pathStr.ends_with(".mov") || pathStr.ends_with(".m4v") ||
        pathStr.ends_with(".flv") || pathStr.ends_with(".wmv") ||
        pathStr.ends_with(".gif")) {
        return MEDIA_TYPE_VIDEO;
    }

    g_logger->log(LOG_DEBUG, "Unknown media type for {}: {}", path, mime);
    return MEDIA_TYPE_UNKNOWN;
}

bool isVideo(const std::string_view& path) {
    eMediaType type = detectMediaType(path);
    return type == MEDIA_TYPE_VIDEO || type == MEDIA_TYPE_GIF || type == MEDIA_TYPE_APNG;
}

bool isAnimated(const std::string_view& path) {
    return isVideo(path);
}

const char* mediaTypeToString(eMediaType type) {
    switch (type) {
        case MEDIA_TYPE_IMAGE: return "image";
        case MEDIA_TYPE_VIDEO: return "video";
        case MEDIA_TYPE_GIF: return "gif";
        case MEDIA_TYPE_APNG: return "apng";
        default: return "unknown";
    }
}

} // namespace MediaDetector
