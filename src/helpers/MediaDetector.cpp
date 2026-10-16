#include "MediaDetector.hpp"
#include "Logger.hpp"
#include "../defines.hpp"

#include <magic.h>
#include <cstring>
#include <algorithm>

namespace MediaDetector {

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
        // Check for GIF specifically
        if (mime == "image/gif") {
            // GIF can be static or animated, treat as GIF for mpv handling
            return MEDIA_TYPE_GIF;
        }
        return MEDIA_TYPE_VIDEO;
    }

    // Check for image types that might be animated
    if (mime == "image/gif") {
        return MEDIA_TYPE_GIF;
    }
    
    if (mime == "image/png" || mime == "image/apng") {
        // APNG detection requires checking file contents
        // For now, let mpv handle it if it's animated
        return MEDIA_TYPE_APNG;
    }

    // Check for WebP (can be animated)
    if (mime == "image/webp") {
        // WebP can be animated, treat as video candidate
        return MEDIA_TYPE_VIDEO;
    }

    // Static images
    if (mime.starts_with("image/")) {
        return MEDIA_TYPE_IMAGE;
    }

    // Additional video container checks by extension if MIME detection failed
    std::string pathStr(path);
    std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), ::tolower);
    
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
