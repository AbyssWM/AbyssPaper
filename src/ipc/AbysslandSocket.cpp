#include "AbysslandSocket.hpp"

#include <pwd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <format>

#include <abyssutils/memory/Casts.hpp>
#include <abyssutils/utils/ScopeGuard.hpp>

using namespace Abyssutils::Memory;

static int getUID() {
    const auto UID   = getuid();
    const auto PWUID = getpwuid(UID);
    return PWUID ? PWUID->pw_uid : UID;
}

static std::string getRuntimeDir() {
    const auto XDG = getenv("XDG_RUNTIME_DIR");

    if (!XDG) {
        const std::string USERID = std::to_string(getUID());
        return "/run/user/" + USERID + "/abyss";
    }

    return std::string{XDG} + "/abyss";
}

std::expected<std::string, std::string> AbysslandSocket::getFromSocket(const std::string& cmd) {
    static const auto HIS = getenv("ABYSSLAND_INSTANCE_SIGNATURE");

    if (!HIS || HIS[0] == '\0')
        return std::unexpected("ABYSSLAND_INSTANCE_SIGNATURE empty: are we under abyssland?");

    const auto SERVERSOCKET = socket(AF_UNIX, SOCK_STREAM, 0);

    if (SERVERSOCKET < 0)
        return std::unexpected("couldn't open a socket (1)");
    Abyssutils::Utils::CScopeGuard socketGuard{[SERVERSOCKET] { close(SERVERSOCKET); }};

    auto       t = timeval{.tv_sec = 5, .tv_usec = 0};
    setsockopt(SERVERSOCKET, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval));

    sockaddr_un serverAddress = {0};
    serverAddress.sun_family  = AF_UNIX;

    std::string socketPath = getRuntimeDir() + "/" + HIS + "/.socket.sock";

    strncpy(serverAddress.sun_path, socketPath.c_str(), sizeof(serverAddress.sun_path) - 1);

    if (connect(SERVERSOCKET, rc<sockaddr*>(&serverAddress), SUN_LEN(&serverAddress)) < 0)
        return std::unexpected(std::format("couldn't connect to the abyssland socket at {}", socketPath));

    auto sizeWritten = write(SERVERSOCKET, cmd.c_str(), cmd.length());

    if (sizeWritten < 0)
        return std::unexpected("couldn't write (4)");

    std::string reply        = "";
    char        buffer[8192] = {0};

    sizeWritten = read(SERVERSOCKET, buffer, 8192);

    if (sizeWritten < 0) {
        if (errno == EWOULDBLOCK)
            return std::unexpected("Abyssland IPC didn't respond in time");
        return std::unexpected("couldn't read (5)");
    }

    reply += std::string(buffer, sizeWritten);

    while (sizeWritten == 8192) {
        sizeWritten = read(SERVERSOCKET, buffer, 8192);
        if (sizeWritten < 0) {
            return std::unexpected("couldn't read (5)");
        }
        reply += std::string(buffer, sizeWritten);
    }

    return reply;
}
