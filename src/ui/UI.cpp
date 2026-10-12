#include "UI.hpp"
#include "../defines.hpp"
#include "../helpers/Logger.hpp"
#include "../helpers/GlobalState.hpp"
#include "../ipc/AbysslandSocket.hpp"
#include "../ipc/IPC.hpp"
#include "../config/WallpaperMatcher.hpp"

#include <algorithm>
#include <random>
#include <abysstoolkit/core/Output.hpp>

#include <abyssutils/string/String.hpp>

CUI::CUI() = default;

CUI::~CUI() {
    m_targets.clear();
}

static std::string_view pruneDesc(const std::string_view& sv) {
    if (sv.contains('('))
        return Abyssutils::String::trim(sv.substr(0, sv.find_last_of('(')));
    return sv;
}

class CWallpaperTarget::CImagesData {
  public:
    CImagesData(Abysstoolkit::eImageFitMode fitMode, std::vector<std::string> images, const int timeout = 0, std::string order = "default") :
        fitMode(fitMode), images(std::move(images)), order(std::move(order)), timeout(timeout > 0 ? timeout : 30) {}

    const Abysstoolkit::eImageFitMode fitMode;
    std::vector<std::string>         images;
    const std::string                order;
    const int                        timeout;

    std::string                      nextImage() {
        if (order == "random-shuffle" && current + 1 >= images.size()) {
            std::random_device rd;
            std::mt19937       g(rd());
            std::shuffle(images.begin(), images.end(), g);
            current = 0;
            return images[current];
        }

        current = (current + 1) % images.size();
        return images[current];
    }

  private:
    size_t current = 0;
};

CWallpaperTarget::CWallpaperTarget(SP<Abysstoolkit::IBackend> backend, SP<Abysstoolkit::IOutput> output, const std::vector<std::string>& path, Abysstoolkit::eImageFitMode fitMode,
                                   const int timeout, const std::string& order) : m_monitorName(output->port()), m_backend(backend) {
    static const auto SPLASH_REPLY = AbysslandSocket::getFromSocket("/splash");

    static const auto PENABLESPLASH = Abyssparse::CSimpleConfigValue<Abyssparse::INT>(g_config->abyssparse(), "splash");
    static const auto PSPLASHOFFSET = Abyssparse::CSimpleConfigValue<Abyssparse::INT>(g_config->abyssparse(), "splash_offset");
    static const auto PSPLASHALPHA  = Abyssparse::CSimpleConfigValue<Abyssparse::FLOAT>(g_config->abyssparse(), "splash_opacity");

    ASSERT(path.size() > 0);

    m_window = Abysstoolkit::CWindowBuilder::begin()
                   ->type(Abysstoolkit::HT_WINDOW_LAYER)
                   ->prefferedOutput(output)
                   ->anchor(0xF)
                   ->layer(0)
                   ->preferredSize({0, 0})
                   ->exclusiveZone(-1)
                   ->appClass("abysspaper")
                   ->commence();

    m_bg = Abysstoolkit::CRectangleBuilder::begin()
               ->size({Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, {1, 1}})
               ->color([] { return Abysstoolkit::CAbyssColor{0xFF000000}; })
               ->commence();
    m_null = Abysstoolkit::CNullBuilder::begin()->size({Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, {1, 1}})->commence();

    m_image = Abysstoolkit::CImageBuilder::begin()
                  ->path(std::string{path.front()})
                  ->size({Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, {1.F, 1.F}})
                  ->sync(true)
                  ->fitMode(fitMode)
                  ->commence();

    m_lastPath = path.front();

    m_image->setPositionMode(Abysstoolkit::IElement::HT_POSITION_ABSOLUTE);
    m_image->setPositionFlag(Abysstoolkit::IElement::HT_POSITION_FLAG_CENTER, true);

    if (path.size() > 1) {
        m_imagesData = makeUnique<CImagesData>(fitMode, std::vector<std::string>(path), timeout, order);
        m_timer =
            m_backend->addTimer(std::chrono::milliseconds(std::chrono::seconds(m_imagesData->timeout)), [this](ASP<Abysstoolkit::CTimer> self, void*) { onRepeatTimer(); }, nullptr);
    }

    m_window->m_rootElement->addChild(m_bg);
    m_window->m_rootElement->addChild(m_null);
    m_null->addChild(m_image);

    if (!SPLASH_REPLY)
        g_logger->log(LOG_ERR, "Can't get splash: {}", SPLASH_REPLY.error());

    if (SPLASH_REPLY && *PENABLESPLASH) {
        m_splash = Abysstoolkit::CTextBuilder::begin()
                       ->text(std::string{SPLASH_REPLY.value()})
                       ->fontSize({Abysstoolkit::CFontSize::HT_FONT_TEXT, 1.15F})
                       ->color([] { return g_ui->backend()->getPalette()->m_colors.text; })
                       ->a(*PSPLASHALPHA)
                       ->commence();
        m_splash->setPositionMode(Abysstoolkit::IElement::HT_POSITION_ABSOLUTE);
        m_splash->setPositionFlag(Abysstoolkit::IElement::HT_POSITION_FLAG_HCENTER, true);
        m_splash->setPositionFlag(Abysstoolkit::IElement::HT_POSITION_FLAG_BOTTOM, true);
        m_splash->setAbsolutePosition({0.F, sc<float>(-*PSPLASHOFFSET)});
        m_null->addChild(m_splash);
    }

    m_window->open();
}

CWallpaperTarget::~CWallpaperTarget() {
    if (m_timer && !m_timer->passed())
        m_timer->cancel();
}

void CWallpaperTarget::onRepeatTimer() {

    ASSERT(m_imagesData);

    m_lastPath = m_imagesData->nextImage();

    m_image->rebuild()
        ->path(std::string{m_lastPath})
        ->size({Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, Abysstoolkit::CDynamicSize::HT_SIZE_PERCENT, {1.F, 1.F}})
        ->sync(true)
        ->fitMode(m_imagesData->fitMode)
        ->commence();

    m_timer =
        m_backend->addTimer(std::chrono::milliseconds(std::chrono::seconds(m_imagesData->timeout)), [this](ASP<Abysstoolkit::CTimer> self, void*) { onRepeatTimer(); }, nullptr);

    IPC::g_IPCSocket->onWallpaperChanged(m_monitorName, m_lastPath);
}

void CUI::registerOutput(const SP<Abysstoolkit::IOutput>& mon) {
    g_matcher->registerOutput(mon->port(), pruneDesc(mon->desc()));
    if (IPC::g_IPCSocket)
        IPC::g_IPCSocket->onNewDisplay(mon->port());
    mon->m_events.removed.listenStatic([this, m = WP<Abysstoolkit::IOutput>{mon}] {
        g_matcher->unregisterOutput(m->port());
        if (IPC::g_IPCSocket)
            IPC::g_IPCSocket->onRemovedDisplay(m->port());
        std::erase_if(m_targets, [&m](const auto& e) { return e->m_monitorName == m->port(); });
    });
}

bool CUI::run() {
    static const auto PENABLEIPC = Abyssparse::CSimpleConfigValue<Abyssparse::INT>(g_config->abyssparse(), "ipc");

    //
    Abysstoolkit::IBackend::SBackendCreationData data;
    data.pLogConnection = makeShared<Abyssutils::CLI::CLoggerConnection>(*g_logger);
    data.pLogConnection->setName("abysstoolkit");
    data.pLogConnection->setLogLevel(g_state->verbose ? LOG_TRACE : LOG_ERR);

    m_backend = Abysstoolkit::IBackend::createWithData(data);

    if (!m_backend)
        return false;

    if (*PENABLEIPC)
        IPC::g_IPCSocket = makeUnique<IPC::CSocket>();

    const auto MONITORS = m_backend->getOutputs();

    for (const auto& m : MONITORS) {
        registerOutput(m);
    }

    m_listeners.newMon = m_backend->m_events.outputAdded.listen([this](SP<Abysstoolkit::IOutput> mon) { registerOutput(mon); });

    g_logger->log(LOG_DEBUG, "Found {} output(s)", MONITORS.size());

    // load the config now, then bind
    for (const auto& m : MONITORS) {
        targetChanged(m);
    }

    m_listeners.targetChanged = g_matcher->m_events.monitorConfigChanged.listen([this](const std::string_view& m) { targetChanged(m); });

    m_backend->enterLoop();

    return true;
}

SP<Abysstoolkit::IBackend> CUI::backend() {
    return m_backend;
}

static Abysstoolkit::eImageFitMode toFitMode(const std::string_view& sv) {
    if (sv.starts_with("contain"))
        return Abysstoolkit::IMAGE_FIT_MODE_CONTAIN;
    if (sv.starts_with("cover"))
        return Abysstoolkit::IMAGE_FIT_MODE_COVER;
    if (sv.starts_with("tile"))
        return Abysstoolkit::IMAGE_FIT_MODE_TILE;
    if (sv.starts_with("fill"))
        return Abysstoolkit::IMAGE_FIT_MODE_STRETCH;
    return Abysstoolkit::IMAGE_FIT_MODE_COVER;
}

void CUI::targetChanged(const std::string_view& monName) {
    const auto               MONITORS = m_backend->getOutputs();
    SP<Abysstoolkit::IOutput> monitor;

    for (const auto& m : MONITORS) {
        if (m->port() != monName)
            continue;

        monitor = m;
    }

    if (!monitor) {
        g_logger->log(LOG_ERR, "targetChanged but {} has no output?", monName);
        return;
    }

    targetChanged(monitor);
}

void CUI::targetChanged(const SP<Abysstoolkit::IOutput>& mon) {
    const auto TARGET = g_matcher->getSetting(mon->port(), pruneDesc(mon->desc()));

    if (!TARGET) {
        g_logger->log(LOG_DEBUG, "Monitor {} has no target: no wp will be created", mon->port());
        return;
    }

    std::erase_if(m_targets, [&mon](const auto& e) { return e->m_monitorName == mon->port(); });

    m_targets.emplace_back(makeShared<CWallpaperTarget>(m_backend, mon, TARGET->get().paths, toFitMode(TARGET->get().fitMode), TARGET->get().timeout, TARGET->get().order));
}

const std::vector<SP<CWallpaperTarget>>& CUI::targets() {
    return m_targets;
}
