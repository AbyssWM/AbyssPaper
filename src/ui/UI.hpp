#pragma once

#include <vector>

#include <abysstoolkit/core/Backend.hpp>
#include <abysstoolkit/core/Timer.hpp>
#include <abysstoolkit/window/Window.hpp>
#include <abysstoolkit/element/Text.hpp>
#include <abysstoolkit/element/Null.hpp>
#include <abysstoolkit/element/Image.hpp>
#include <abysstoolkit/element/Rectangle.hpp>

#include <abyssutils/signal/Listener.hpp>

#include "../helpers/Memory.hpp"
#include "VideoElement.hpp"

class CWallpaperTarget {
  public:
    CWallpaperTarget(SP<Abysstoolkit::IBackend> backend, SP<Abysstoolkit::IOutput> output, const std::vector<std::string>& path,
                     Abysstoolkit::eImageFitMode fitMode = Abysstoolkit::IMAGE_FIT_MODE_COVER, const int timeout = 0, const std::string& order = "default");
    ~CWallpaperTarget();

    CWallpaperTarget(const CWallpaperTarget&) = delete;
    CWallpaperTarget(CWallpaperTarget&)       = delete;
    CWallpaperTarget(CWallpaperTarget&&)      = delete;

    std::string m_monitorName, m_lastPath;

  private:
    void onRepeatTimer();

    class CImagesData;

    UP<CImagesData>                    m_imagesData;
    ASP<Abysstoolkit::CTimer>           m_timer;
    SP<Abysstoolkit::IBackend>          m_backend;
    SP<Abysstoolkit::IWindow>           m_window;
    SP<Abysstoolkit::CNullElement>      m_null;
    SP<Abysstoolkit::CRectangleElement> m_bg;
    SP<Abysstoolkit::CImageElement>     m_image;
    SP<Abysstoolkit::CTextElement>      m_splash;
    UP<CVideoElement>                   m_video;
    bool                                m_isVideo = false;
};

class CUI {
  public:
    CUI();
    ~CUI();

    bool                                     run();
    SP<Abysstoolkit::IBackend>                backend();
    const std::vector<SP<CWallpaperTarget>>& targets();

  private:
    void                              targetChanged(const SP<Abysstoolkit::IOutput>& mon);
    void                              targetChanged(const std::string_view& monName);
    void                              registerOutput(const SP<Abysstoolkit::IOutput>& mon);

    SP<Abysstoolkit::IBackend>         m_backend;

    std::vector<SP<CWallpaperTarget>> m_targets;

    struct {
        Abyssutils::Signal::CAbyssSignalListener targetChanged;
        Abyssutils::Signal::CAbyssSignalListener newMon;
    } m_listeners;
};

inline UP<CUI> g_ui;
