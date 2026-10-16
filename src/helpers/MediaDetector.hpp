#pragma once

#include <string>
#include <string_view>

namespace MediaDetector {
    enum eMediaType {
        MEDIA_TYPE_UNKNOWN = 0,
        MEDIA_TYPE_IMAGE,      // static images (png, jpg, webp, etc.)
        MEDIA_TYPE_VIDEO,      // video files (mp4, webm, mkv, etc.)
        MEDIA_TYPE_GIF,        // animated GIF
        MEDIA_TYPE_APNG        // animated PNG
    };

    eMediaType detectMediaType(const std::string_view& path);
    bool       isVideo(const std::string_view& path);
    bool       isAnimated(const std::string_view& path);
    const char* mediaTypeToString(eMediaType type);
}
