#include "VideoElement.hpp"
#include "../helpers/Logger.hpp"
#include "../defines.hpp"

#include <abysstoolkit/core/Backend.hpp>
#include <abysstoolkit/core/Output.hpp>

#include <cstring>
#include <cstdlib>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <mpv/render_gl.h>

UP<CVideoElement> CVideoElement::create(const SCreateInfo& info) {
    auto elem = UP<CVideoElement>(new CVideoElement());
    if (!elem->init(info))
        return nullptr;
    return elem;
}

CVideoElement::~CVideoElement() {
    cleanupRender();
    if (m_mpvRenderCtx) {
        mpv_render_context_free(m_mpvRenderCtx);
        m_mpvRenderCtx = nullptr;
    }
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

void CVideoElement::mpvWakeupCallback(void* ctx) {
    auto* self = static_cast<CVideoElement*>(ctx);
    if (self)
        self->processMPVEvents();
}

void CVideoElement::mpvUpdateCallback(void* ctx) {
    auto* self = static_cast<CVideoElement*>(ctx);
    if (self && self->m_updateCallback)
        self->m_updateCallback();
}

void CVideoElement::processMPVEvents() {
    while (m_mpv) {
        mpv_event* event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;

        switch (event->event_id) {
            case MPV_EVENT_PLAYBACK_RESTART:
                m_isPlaying = true;
                g_logger->log(LOG_DEBUG, "[mpv] Playback started");
                break;
            case MPV_EVENT_END_FILE: {
                auto* endFile = static_cast<mpv_event_end_file*>(event->data);
                if (endFile->reason == MPV_END_FILE_REASON_ERROR) {
                    g_logger->log(LOG_ERR, "[mpv] Playback error: {}", mpv_error_string(endFile->error));
                }
                break;
            }
            case MPV_EVENT_LOG_MESSAGE: {
                auto* msg = static_cast<mpv_event_log_message*>(event->data);
                if (strcmp(msg->level, "error") == 0) {
                    g_logger->log(LOG_ERR, "[mpv] {}: {}", msg->prefix, msg->text);
                }
                break;
            }
            case MPV_EVENT_PROPERTY_CHANGE: {
                auto* prop = static_cast<mpv_event_property*>(event->data);
                if (prop->name && strcmp(prop->name, "pause") == 0 && prop->format == MPV_FORMAT_FLAG) {
                    int paused = *(int*)prop->data;
                    m_isPlaying = !paused;
                    g_logger->log(LOG_DEBUG, "[mpv] {}", paused ? "Paused" : "Resumed");
                }
                break;
            }
            default:
                break;
        }
    }
}

bool CVideoElement::init(const SCreateInfo& info) {
    m_fitMode = info.fitMode;
    m_audioEnabled = info.audio;
    m_updateCallback = info.updateCallback;
    m_backend = info.backend;

    if (!initMPV(info)) {
        g_logger->log(LOG_ERR, "Failed to initialize MPV for {}", info.path);
        return false;
    }

    // Try OpenGL first (Stufe B), fallback to Software (Stufe A)
    if (initOpenGLRender(info)) {
        g_logger->log(LOG_DEBUG, "Video initialized with OpenGL rendering (zero-copy)");
        return true;
    }

    g_logger->log(LOG_WARN, "OpenGL rendering failed, falling back to software rendering");
    if (initSoftwareRender(info)) {
        g_logger->log(LOG_DEBUG, "Video initialized with software rendering (wl_shm)");
        return true;
    }

    g_logger->log(LOG_ERR, "Failed to initialize any rendering mode");
    return false;
}

bool CVideoElement::initMPV(const SCreateInfo& info) {
    m_mpv = mpv_create();
    if (!m_mpv) {
        g_logger->log(LOG_ERR, "Failed to create MPV context");
        return false;
    }

    // Basic options
    mpv_set_option_string(m_mpv, "vo", "null"); // We handle rendering
    mpv_set_option_string(m_mpv, "hwdec", "auto-safe"); // vaapi, nvdec auto-detection
    mpv_set_option_string(m_mpv, "video-sync", "audio");
    mpv_set_option_string(m_mpv, "loop-file", info.loop ? "inf" : "no");
    mpv_set_option_string(m_mpv, "keep-open", "yes");
    mpv_set_option_string(m_mpv, "force-window", "no");
    mpv_set_option_string(m_mpv, "input-default-bindings", "no");
    mpv_set_option_string(m_mpv, "input-vo-keyboard", "no");
    mpv_set_option_string(m_mpv, "osc", "no");

    // Audio setup
    if (info.audio) {
        mpv_set_option_string(m_mpv, "ao", "pipewire,pulse,alsa");
        mpv_set_option_string(m_mpv, "volume", "100");
    } else {
        mpv_set_option_string(m_mpv, "ao", "null");
        mpv_set_option_string(m_mpv, "mute", "yes");
    }

    // Performance tuning (from plan)
    mpv_set_option_string(m_mpv, "demuxer-max-bytes", "16MiB");
    mpv_set_option_string(m_mpv, "cache", "no");
    mpv_set_option_string(m_mpv, "vd-lavc-threads", "2");
    mpv_set_option_string(m_mpv, "framedrop", "vo");

    // Logging
    mpv_request_log_messages(m_mpv, "warn");

    // Observe pause property for event tracking
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);

    if (mpv_initialize(m_mpv) < 0) {
        g_logger->log(LOG_ERR, "Failed to initialize MPV");
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
        return false;
    }

    // Set callbacks
    mpv_set_wakeup_callback(m_mpv, mpvWakeupCallback, this);

    // Load file
    const char* cmd[] = {"loadfile", info.path.c_str(), nullptr};
    if (mpv_command(m_mpv, cmd) < 0) {
        g_logger->log(LOG_ERR, "Failed to load file: {}", info.path);
        return false;
    }

    // Check for audio streams
    mpv_node audioStreams;
    if (mpv_get_property(m_mpv, "track-list", MPV_FORMAT_NODE, &audioStreams) == 0) {
        m_hasAudio = false;
        if (audioStreams.format == MPV_FORMAT_NODE_ARRAY) {
            for (int i = 0; i < audioStreams.u.list->num; i++) {
                mpv_node* track = &audioStreams.u.list->values[i];
                if (track->format == MPV_FORMAT_NODE_MAP) {
                    for (int j = 0; j < track->u.list->num; j++) {
                        if (strcmp(track->u.list->keys[j], "type") == 0 &&
                            track->u.list->values[j].format == MPV_FORMAT_STRING &&
                            strcmp(track->u.list->values[j].u.string, "audio") == 0) {
                            m_hasAudio = true;
                            break;
                        }
                    }
                }
            }
        }
        mpv_free_node_contents(&audioStreams);
    }

    g_logger->log(LOG_DEBUG, "MPV initialized for {} (audio: {})", info.path, m_hasAudio);
    return true;
}

static void* get_proc_address_mpv(void* ctx, const char* name) {
    return reinterpret_cast<void*>(eglGetProcAddress(name));
}

bool CVideoElement::initOpenGLRender(const SCreateInfo& info) {
    // TODO: Get Wayland display/surface from backend/output
    // For now, we'll use the existing toolkit integration
    
    // Initialize EGL
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplay == EGL_NO_DISPLAY) {
        g_logger->log(LOG_ERR, "Failed to get EGL display");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(static_cast<EGLDisplay>(m_eglDisplay), &major, &minor)) {
        g_logger->log(LOG_ERR, "Failed to initialize EGL");
        m_eglDisplay = nullptr;
        return false;
    }

    g_logger->log(LOG_DEBUG, "EGL {}.{} initialized", major, minor);

    // Choose EGL config
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    EGLConfig eglConfig;
    EGLint numConfigs;
    if (!eglChooseConfig(static_cast<EGLDisplay>(m_eglDisplay), configAttribs, &eglConfig, 1, &numConfigs) || numConfigs == 0) {
        g_logger->log(LOG_ERR, "Failed to choose EGL config");
        eglTerminate(static_cast<EGLDisplay>(m_eglDisplay));
        m_eglDisplay = nullptr;
        return false;
    }

    // Create EGL context
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    m_eglContext = eglCreateContext(static_cast<EGLDisplay>(m_eglDisplay), eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (m_eglContext == EGL_NO_CONTEXT) {
        g_logger->log(LOG_ERR, "Failed to create EGL context");
        eglTerminate(static_cast<EGLDisplay>(m_eglDisplay));
        m_eglDisplay = nullptr;
        return false;
    }

    // Create MPV OpenGL render context
    mpv_opengl_init_params gl_init_params{};
    gl_init_params.get_proc_address = get_proc_address_mpv;
    gl_init_params.get_proc_address_ctx = nullptr;

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&m_mpvRenderCtx, m_mpv, params) < 0) {
        g_logger->log(LOG_ERR, "Failed to create MPV OpenGL render context");
        eglDestroyContext(static_cast<EGLDisplay>(m_eglDisplay), static_cast<EGLContext>(m_eglContext));
        eglTerminate(static_cast<EGLDisplay>(m_eglDisplay));
        m_eglContext = nullptr;
        m_eglDisplay = nullptr;
        return false;
    }

    mpv_render_context_set_update_callback(m_mpvRenderCtx, mpvUpdateCallback, this);

    m_renderMode = RENDER_MODE_OPENGL;
    g_logger->log(LOG_DEBUG, "OpenGL render context created successfully");
    return true;
}

bool CVideoElement::initSoftwareRender(const SCreateInfo& info) {
    // Software rendering using MPV's 'sw' render API
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>("sw")},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&m_mpvRenderCtx, m_mpv, params) < 0) {
        g_logger->log(LOG_ERR, "Failed to create MPV software render context");
        return false;
    }

    mpv_render_context_set_update_callback(m_mpvRenderCtx, mpvUpdateCallback, this);

    m_renderMode = RENDER_MODE_SOFTWARE;
    g_logger->log(LOG_DEBUG, "Software render context created successfully");
    return true;
}

void CVideoElement::cleanupRender() {
    if (m_eglContext) {
        eglDestroyContext(static_cast<EGLDisplay>(m_eglDisplay), static_cast<EGLContext>(m_eglContext));
        m_eglContext = nullptr;
    }
    if (m_eglDisplay) {
        eglTerminate(static_cast<EGLDisplay>(m_eglDisplay));
        m_eglDisplay = nullptr;
    }
    if (m_shmBuffer) {
        free(m_shmBuffer);
        m_shmBuffer = nullptr;
        m_shmSize = 0;
    }
}

void CVideoElement::render() {
    if (!m_mpvRenderCtx)
        return;

    if (m_renderMode == RENDER_MODE_OPENGL) {
        // OpenGL rendering
        mpv_opengl_fbo fbo{};
        fbo.fbo = 0; // Default framebuffer
        fbo.w = m_width;
        fbo.h = m_height;
        fbo.internal_format = GL_RGBA8;

        int flip = 1;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
            {MPV_RENDER_PARAM_FLIP_Y, &flip},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        mpv_render_context_render(m_mpvRenderCtx, params);
    } else if (m_renderMode == RENDER_MODE_SOFTWARE) {
        // Software rendering to wl_shm buffer
        if (m_width <= 0 || m_height <= 0)
            return;

        size_t requiredSize = m_width * m_height * 4; // BGRA
        if (m_shmSize < requiredSize) {
            m_shmBuffer = realloc(m_shmBuffer, requiredSize);
            m_shmSize = requiredSize;
        }

        int size[2] = {m_width, m_height};
        size_t stride = static_cast<size_t>(m_width * 4);

        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_SW_SIZE, size},
            {MPV_RENDER_PARAM_SW_FORMAT, const_cast<char*>("bgra")},
            {MPV_RENDER_PARAM_SW_STRIDE, &stride},
            {MPV_RENDER_PARAM_SW_POINTER, m_shmBuffer},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        mpv_render_context_render(m_mpvRenderCtx, params);
    }
}

void CVideoElement::resize(int width, int height) {
    m_width = width;
    m_height = height;
    g_logger->log(LOG_DEBUG, "Video resized to {}x{}", width, height);
}

void CVideoElement::pause() {
    if (m_mpv) {
        int flag = 1;
        mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &flag);
    }
}

void CVideoElement::resume() {
    if (m_mpv) {
        int flag = 0;
        mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &flag);
    }
}

void CVideoElement::setMute(bool mute) {
    if (m_mpv) {
        int flag = mute ? 1 : 0;
        mpv_set_property(m_mpv, "mute", MPV_FORMAT_FLAG, &flag);
    }
}

void CVideoElement::setVolume(int volume) {
    if (m_mpv) {
        double vol = std::clamp(volume, 0, 100);
        mpv_set_property(m_mpv, "volume", MPV_FORMAT_DOUBLE, &vol);
    }
}

bool CVideoElement::isPlaying() const {
    return m_isPlaying;
}

bool CVideoElement::hasAudio() const {
    return m_hasAudio;
}
