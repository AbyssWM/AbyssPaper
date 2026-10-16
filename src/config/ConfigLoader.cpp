#include "ConfigLoader.hpp"
#include "../defines.hpp"
#include "../helpers/Logger.hpp"

// Include generated configs (if available)
#ifdef ABYSS_CONFIG_AVAILABLE
#include "abyspaperConfigs.hpp"
#endif

namespace abyss::paper {

bool CConfigLoader::hasConfigs() {
#ifdef ABYSS_CONFIG_AVAILABLE
    return NUM_CONFIGS > 0;
#else
    return false;
#endif
}

std::string CConfigLoader::getWallpaper() {
#ifdef ABYSS_CONFIG_AVAILABLE
    if (NUM_CONFIGS > 0) {
        // Use first config (example-paper namespace)
        // In future: support multiple configs or selection
        return std::string(example_paper::wallpaper);
    }
#endif
    return "";
}

bool CConfigLoader::getAudio() {
#ifdef ABYSS_CONFIG_AVAILABLE
    if (NUM_CONFIGS > 0) {
        return example_paper::audio;
    }
#endif
    return false;
}

int CConfigLoader::getVolume() {
#ifdef ABYSS_CONFIG_AVAILABLE
    if (NUM_CONFIGS > 0) {
        return example_paper::volume;
    }
#endif
    return 50; // Default
}

void CConfigLoader::printConfigInfo() {
#ifdef ABYSS_CONFIG_AVAILABLE
    g_logger->log(LOG_DEBUG, "#include.abyss: {} config(s) loaded", NUM_CONFIGS);
    
    if (NUM_CONFIGS > 0) {
        for (int i = 0; i < NUM_CONFIGS; i++) {
            g_logger->log(LOG_DEBUG, "  - {}", AVAILABLE_CONFIGS[i]);
        }
        
        // Print first config details
        g_logger->log(LOG_DEBUG, "Using config: {}", AVAILABLE_CONFIGS[0]);
        g_logger->log(LOG_DEBUG, "  wallpaper: {}", example_paper::wallpaper);
        g_logger->log(LOG_DEBUG, "  audio: {}", example_paper::audio);
        g_logger->log(LOG_DEBUG, "  volume: {}", example_paper::volume);
    }
#else
    g_logger->log(LOG_DEBUG, "#include.abyss: No configs available (using legacy system)");
#endif
}

} // namespace abyss::paper
