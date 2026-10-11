#pragma once

#include "WPSchemeHandlerFactory.h"
#include "WallpaperEngine/WebBrowser/WebBrowserContext.h"
#include "include/cef_app.h"
#include "include/cef_render_process_handler.h"
#include "include/cef_v8.h"

namespace WallpaperEngine::Application {
class WallpaperApplication;
}

namespace WallpaperEngine::WebBrowser::CEF {
class SubprocessApp : public CefApp, public CefRenderProcessHandler, public CefV8Handler {
public:
    explicit SubprocessApp (WallpaperEngine::Application::WallpaperApplication& application);

    void OnRegisterCustomSchemes (CefRawPtr<CefSchemeRegistrar> registrar) override;

    // CefRenderProcessHandler
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler () override { return this; }
    void OnWebKitInitialized () override;

    // CefV8Handler, unused: the audio extension is pure javascript and has no native functions
    bool Execute (
	const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval, CefString& exception
    ) override;

protected:
    const WallpaperEngine::Application::WallpaperApplication& getApplication () const;
    const std::map<std::string, WPSchemeHandlerFactory*>& getHandlerFactories () const;

private:
    std::map<std::string, WPSchemeHandlerFactory*> m_handlerFactories = {};
    WallpaperEngine::Application::WallpaperApplication& m_application;
    IMPLEMENT_REFCOUNTING (SubprocessApp);
    DISALLOW_COPY_AND_ASSIGN (SubprocessApp);
};
}