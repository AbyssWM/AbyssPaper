#include "defines.hpp"
#include "helpers/Logger.hpp"
#include "helpers/GlobalState.hpp"
#include "ui/UI.hpp"
#include "config/ConfigManager.hpp"
#include "config/ConfigLoader.hpp"

#include <abyssutils/cli/ArgumentParser.hpp>

using namespace Abyssutils::CLI;

int main(int argc, const char** argv, const char** envp) {

    CArgumentParser parser({argv, argc});

    ASSERT(parser.registerStringOption("config", "c", "Set a custom config path"));
    ASSERT(parser.registerBoolOption("verbose", "", "Enable more logging"));
    ASSERT(parser.registerBoolOption("version", "v", "Show abysspaper's version"));
    ASSERT(parser.registerBoolOption("help", "h", "Show the help menu"));

    if (const auto ret = parser.parse(); !ret) {
        g_logger->log(LOG_ERR, "Failed parsing arguments: {}", ret.error());
        return 1;
    }

    if (parser.getBool("help").value_or(false)) {
        std::println("{}", parser.getDescription(std::format("abysspaper v{}", ABYSSPAPER_VERSION)));
        return 0;
    }

    if (parser.getBool("version").value_or(false)) {
        std::println("abysspaper v{}", ABYSSPAPER_VERSION);
        return 0;
    }

    if (parser.getBool("verbose").value_or(false)) {
        g_logger->setLogLevel(LOG_TRACE);
        g_state->verbose = true;
    }

    g_logger->log(LOG_DEBUG, "Welcome to abysspaper!\nbuilt from commit {} ({})", GIT_COMMIT_HASH, GIT_COMMIT_MESSAGE);

    // Test new #include.abyss config system
    abyss::paper::CConfigLoader::printConfigInfo();

    g_config = makeUnique<CConfigManager>(std::string{parser.getString("config").value_or("")});
    if (!g_config->init())
        return 1;

    g_ui = makeUnique<CUI>();
    return g_ui->run() ? 0 : 1;
}
