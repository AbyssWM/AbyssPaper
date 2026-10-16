#include "src/config/ConfigManager.hpp"
#include "src/config/WallpaperMatcher.hpp"
#include "src/helpers/MediaDetector.hpp"
#include "src/ipc/AbysslandSocket.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>

static int failures = 0;
static void check(bool passed, const std::string& message) {
    if (!passed) {
        std::cerr << message << '\n';
        ++failures;
    }
}

int main(int argc, char** argv) {
    if (argc != 2)
        return 2;
    const auto directory = std::filesystem::absolute(argv[1]);
    for (const auto& [name, expected] : {
             std::pair{"static.png", MediaDetector::MEDIA_TYPE_IMAGE},
             {"animated.png", MediaDetector::MEDIA_TYPE_APNG},
             {"static.webp", MediaDetector::MEDIA_TYPE_IMAGE},
             {"animated.webp", MediaDetector::MEDIA_TYPE_VIDEO},
             {"animated.gif", MediaDetector::MEDIA_TYPE_GIF},
             {"clip.mp4", MediaDetector::MEDIA_TYPE_VIDEO},
             {"unrelated.txt", MediaDetector::MEDIA_TYPE_UNKNOWN}}) {
        check(MediaDetector::detectMediaType((directory / name).string()) == expected,
              std::string{"Incorrect media type: "} + name);
    }
    check(!MediaDetector::isVideo((directory / "static.png").string()), "Static PNG selected video renderer");
    check(!MediaDetector::isVideo((directory / "missing.png").string()), "Missing file accepted as video");

    for (const auto& name : {"clip.mp4", "static.png", "."}) {
        const auto config = directory / "paper.conf";
        {
            std::ofstream file(config);
            file << "splash = 0\nipc = 0\nwallpaper {\nmonitor = DP-1\npath = "
                 << (directory / name).string() << "\nfit_mode = contain\naudio = true\n}\n";
        }
        g_matcher = makeUnique<CWallpaperMatcher>();
        g_config = makeUnique<CConfigManager>(config.string());
        check(g_config->init(), "Config parse failed");
        const auto settings = g_config->getSettings();
        check(settings.size() == 1, std::string{"Media rejected in config: "} + name);
        if (settings.size() != 1)
            continue;
        check(settings.front().audio, "Audio setting lost");
        if (std::string_view{name} == ".") {
            check(settings.front().paths.size() == 6, "Mixed directory must include all images and videos, excluding text/config");
        } else {
            check(settings.front().paths == std::vector{(directory / name).string()}, "Incorrect resolved media path");
        }
    }

    const auto includes = directory / "includes";
    std::filesystem::create_directories(includes / "nested");
    {
        std::ofstream(includes / "main.conf") << "source = nested/*.conf\n";
        std::ofstream(includes / "nested" / "a.conf") << "source = ../wallpaper.conf\n";
        std::ofstream(includes / "wallpaper.conf")
            << "wallpaper {\nmonitor = DP-2\npath = " << (directory / "static.png").string() << "\n}\n";
    }
    g_matcher = makeUnique<CWallpaperMatcher>();
    g_config = makeUnique<CConfigManager>((includes / "main.conf").string());
    check(g_config->init(), "Glob and nested relative source failed");
    check(g_config->getSettings().size() == 1, "Nested source did not apply wallpaper");
    {
        std::ofstream(includes / "nested" / "a.conf") << "ipc = invalid_integer\n";
    }
    g_config = makeUnique<CConfigManager>((includes / "main.conf").string());
    check(!g_config->init(), "Invalid source must fail config initialization");
    {
        std::ofstream(includes / "nested" / "a.conf") << "source = ../main.conf\n";
    }
    g_config = makeUnique<CConfigManager>((includes / "main.conf").string());
    check(!g_config->init(), "Include cycle was not rejected");

    // Connect only to a nonexistent test socket, never the active compositor.
    setenv("XDG_RUNTIME_DIR", directory.c_str(), 1);
    setenv("ABYSSLAND_INSTANCE_SIGNATURE", "paper-regression-test", 1);
    const auto countFDs = [] {
        return std::distance(std::filesystem::directory_iterator("/proc/self/fd"), std::filesystem::directory_iterator{});
    };
    const auto before = countFDs();
    for (int i = 0; i < 16; ++i)
        check(!AbysslandSocket::getFromSocket("/splash"), "Unexpected test socket connection");
    check(countFDs() == before, "Failed IPC connection leaked file descriptors");
    return failures ? 1 : 0;
}
