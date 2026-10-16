#pragma once

#include <string>
#include <optional>

namespace abyss::paper {

/**
 * ConfigLoader - Simple loader for #include.abyss configs
 * Provides access to compile-time generated configs
 */
class CConfigLoader {
  public:
    CConfigLoader() = default;

    /**
     * Check if any configs are available
     */
    static bool hasConfigs();

    /**
     * Get wallpaper path from first available config
     * Falls back to empty string if no configs found
     */
    static std::string getWallpaper();

    /**
     * Get audio setting from first available config
     * Falls back to false if no configs found
     */
    static bool getAudio();

    /**
     * Get volume setting from first available config
     * Falls back to 50 if no configs found
     */
    static int getVolume();

    /**
     * Print info about available configs
     */
    static void printConfigInfo();
};

} // namespace abyss::paper
