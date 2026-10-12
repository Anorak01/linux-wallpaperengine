#include "RenderHandler.h"

#include "WallpaperEngine/Logging/Log.h"

#include <chrono>
#include <cstdlib>

using namespace WallpaperEngine::WebBrowser::CEF;

RenderHandler::RenderHandler (WallpaperEngine::Render::Wallpapers::CWeb* webdata) : m_webdata (webdata) { }

// Required by CEF
void RenderHandler::GetViewRect (CefRefPtr<CefBrowser> browser, CefRect& rect) {
    rect = CefRect (0, 0, this->m_webdata->getWidth (), this->m_webdata->getHeight ());
}

// Will be executed in CEF message loop
void RenderHandler::OnPaint (
    CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer,
    const int width, const int height
) {
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, this->texture ());

    // re-allocating the texture on every frame is expensive: only (re)allocate when the
    // size changed, otherwise update the existing storage in place
    if (width != this->m_lastPaintWidth || height != this->m_lastPaintHeight) {
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
	this->m_lastPaintWidth = width;
	this->m_lastPaintHeight = height;
    } else {
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
    }

    glBindTexture (GL_TEXTURE_2D, 0);

    static const bool perfLog = std::getenv ("LWE_PERF") != nullptr;

    if (perfLog) {
	static uint32_t perfFrames = 0;
	static auto perfLastReport = std::chrono::steady_clock::now ();
	const auto now = std::chrono::steady_clock::now ();

	perfFrames++;

	if (now - perfLastReport >= std::chrono::seconds (1)) {
	    sLog.out ("[perf] CEF OnPaint rate: ", perfFrames, " fps");
	    perfFrames = 0;
	    perfLastReport = now;
	}
    }
}

int RenderHandler::getWidth () const { return this->m_webdata->getWidth (); }

int RenderHandler::getHeight () const { return this->m_webdata->getHeight (); }

GLuint RenderHandler::texture () const { return this->m_webdata->getWallpaperTexture (); }