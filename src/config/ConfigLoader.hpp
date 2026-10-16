#pragma once

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
     * Print info about available configs
     */
    static void printConfigInfo();
};

} // namespace abyss::paper
