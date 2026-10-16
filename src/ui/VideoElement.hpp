#pragma once

#include <memory>
#include <string>
#include <functional>
#include <mpv/client.h>
#include <mpv/render.h>

#include "../helpers/Memory.hpp"

namespace Abysstoolkit {
    class IOutput;
    class IBackend;
}

class CVideoElement {
  public:
    enum eRenderMode {
        RENDER_MODE_NONE = 0,
        RENDER_MODE_SOFTWARE,  // wl_shm (Stufe A)
        RENDER_MODE_OPENGL     // EGL/GLES3 (Stufe B)
    };

    enum eFitMode {
        FIT_MODE_COVER = 0,
        FIT_MODE_CONTAIN,
        FIT_MODE_FILL,
        FIT_MODE_TILE
    };

    struct SCreateInfo {
        std::string                   path;
        eFitMode                      fitMode   = FIT_MODE_COVER;
        bool                          audio     = false;
        bool                          loop      = true;
        SP<Abysstoolkit::IOutput>     output;
        SP<Abysstoolkit::IBackend>    backend;
        std::function<void()>         updateCallback;
    };

    static UP<CVideoElement> create(const SCreateInfo& info);

    ~CVideoElement();

    void render();
    void resize(int width, int height);
    void pause();
    void resume();
    void setMute(bool mute);
    void setVolume(int volume); // 0-100

    bool isPlaying() const;
    bool hasAudio() const;
    eRenderMode getRenderMode() const { return m_renderMode; }

    // Wayland surface integration
    void* getWaylandSurface() const { return m_wlSurface; }
    void* getEGLWindow() const { return m_eglWindow; }

  private:
    CVideoElement() = default;
    bool init(const SCreateInfo& info);
    bool initMPV(const SCreateInfo& info);
    bool initOpenGLRender(const SCreateInfo& info);
    bool initSoftwareRender(const SCreateInfo& info);
    void cleanupRender();

    static void mpvWakeupCallback(void* ctx);
    static void mpvUpdateCallback(void* ctx);
    void processMPVEvents();

    mpv_handle*         m_mpv           = nullptr;
    mpv_render_context* m_mpvRenderCtx  = nullptr;
    eRenderMode         m_renderMode    = RENDER_MODE_NONE;
    eFitMode            m_fitMode       = FIT_MODE_COVER;

    bool                m_hasAudio      = false;
    bool                m_audioEnabled  = false;
    bool                m_isPlaying     = false;
    int                 m_width         = 0;
    int                 m_height        = 0;

    // OpenGL (Stufe B)
    void*               m_eglDisplay    = nullptr;
    void*               m_eglContext    = nullptr;
    void*               m_eglSurface    = nullptr;
    void*               m_eglWindow     = nullptr;

    // Software (Stufe A)
    void*               m_shmBuffer     = nullptr;
    size_t              m_shmSize       = 0;

    // Wayland
    void*               m_wlDisplay     = nullptr;
    void*               m_wlSurface     = nullptr;
    void*               m_wlCompositor  = nullptr;

    std::function<void()> m_updateCallback;
    SP<Abysstoolkit::IBackend> m_backend;
};
