#include "SubprocessApp.h"
#include "WPSchemeHandlerFactory.h"
#include "WallpaperEngine/Data/Model/Project.h"

using namespace WallpaperEngine::WebBrowser::CEF;

SubprocessApp::SubprocessApp (WallpaperEngine::Application::WallpaperApplication& application) :
    m_application (application) {
    for (const auto& info : this->m_application.getBackgrounds () | std::views::values) {
	this->m_handlerFactories[info->workshopId] = new WPSchemeHandlerFactory (*info);
    }
}

void SubprocessApp::OnRegisterCustomSchemes (CefRawPtr<CefSchemeRegistrar> registrar) {
    // register all the needed schemes, "wp" + the background id is going to be our scheme
    for (const auto& workshopId : this->m_handlerFactories | std::views::keys) {
	registrar->AddCustomScheme (
	    WPSchemeHandlerFactory::generateSchemeName (workshopId),
	    CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE | CEF_SCHEME_OPTION_FETCH_ENABLED
	);
    }
}

const WallpaperEngine::Application::WallpaperApplication& SubprocessApp::getApplication () const {
    return this->m_application;
}

const std::map<std::string, WPSchemeHandlerFactory*>& SubprocessApp::getHandlerFactories () const {
    return this->m_handlerFactories;
}

void SubprocessApp::OnWebKitInitialized () {
    // register a small javascript extension that stores the callback the wallpaper registers,
    // mimicking wallpaper engine's window.wallpaperRegisterAudioListener API for web wallpapers.
    // the actual audio data is pushed from the browser process, which calls
    // __wallpaperEngineDispatchAudio with the spectrum values through ExecuteJavaScript
    // NOTE: extension code runs in a restricted scope where `window` is NOT accessible:
    // referencing it crashes the renderer with SIGTRAP during context creation. bare
    // top-level declarations are shared with the page, which is how the API is exposed
    const std::string extensionCode = R"EXT(
var __wallpaperEngineAudioListener = null;
function wallpaperRegisterAudioListener (callback) {
    __wallpaperEngineAudioListener = callback;
};
function __wallpaperEngineDispatchAudio (audio) {
    if (typeof __wallpaperEngineAudioListener === 'function') {
        __wallpaperEngineAudioListener (audio);
    }
};
)EXT";

    CefRegisterExtension ("wallpaperengine/audio", extensionCode, this);
}

bool SubprocessApp::Execute (
    const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval, CefString& exception
) {
    // there are no native functions in the audio extension, nothing to handle
    return false;
}