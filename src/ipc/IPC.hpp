#pragma once

#include "../helpers/Memory.hpp"

#include <abysswire/abysswire.hpp>
#include <abysspaper_core-server.hpp>

namespace IPC {
    class CWallpaperObject {
      public:
        CWallpaperObject(SP<CAbysspaperWallpaperObject>&& obj);
        ~CWallpaperObject() = default;

      private:
        void                          apply();

        SP<CAbysspaperWallpaperObject> m_object;

        std::string                   m_path;
        abysspaperCoreWallpaperFitMode m_fitMode = ABYSSPAPER_CORE_WALLPAPER_FIT_MODE_COVER;
        std::string                   m_monitor;

        bool                          m_inert = false;
    };

    class CSocket {
      public:
        CSocket();
        ~CSocket() = default;

        void onNewDisplay(const std::string& sv);
        void onRemovedDisplay(const std::string& sv);
        void onWallpaperChanged(const std::string& mon, const std::string& path);

      private:
        SP<Abysswire::IServerSocket>                  m_socket;

        std::string                                  m_socketPath = "";

        std::vector<SP<CAbysspaperCoreManagerObject>> m_managers;
        std::vector<SP<CWallpaperObject>>            m_wallpaperObjects;
        std::vector<SP<CAbysspaperStatusObject>>      m_statusObjects;

        friend class CWallpaperObject;
    };

    inline UP<CSocket> g_IPCSocket;
};