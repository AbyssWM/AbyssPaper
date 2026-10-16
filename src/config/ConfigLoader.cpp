#include "ConfigLoader.hpp"
#include "../defines.hpp"
#include "../helpers/Logger.hpp"

// Include generated configs (if available)
#ifdef ABYSS_CONFIG_AVAILABLE
#include "abysspaperConfigs.hpp"
#endif

namespace abyss::paper {

bool CConfigLoader::hasConfigs() {
#ifdef ABYSS_CONFIG_AVAILABLE
    return NUM_CONFIGS > 0;
#else
    return false;
#endif
}

void CConfigLoader::printConfigInfo() {
#ifdef ABYSS_CONFIG_AVAILABLE
    g_logger->log(LOG_DEBUG, "#include.abyss: {} config(s) compiled; runtime settings still use abysspaper.conf", NUM_CONFIGS);
    
    if (NUM_CONFIGS > 0) {
        for (int i = 0; i < NUM_CONFIGS; i++) {
            g_logger->log(LOG_DEBUG, "  - {}", AVAILABLE_CONFIGS[i]);
        }
        
    }
#else
    g_logger->log(LOG_DEBUG, "#include.abyss: No configs available (using legacy system)");
#endif
}

} // namespace abyss::paper
