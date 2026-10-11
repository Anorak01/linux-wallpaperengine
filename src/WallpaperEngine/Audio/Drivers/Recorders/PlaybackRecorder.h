#pragma once

#include <glm/vec2.hpp>

namespace WallpaperEngine::Audio::Drivers::Recorders {
class PlaybackRecorder {
public:
    virtual ~PlaybackRecorder () = default;

    virtual void update ();

    /**
     * @return true if real audio data is being captured. When false the spectrum
     *         stays silent and audio-reactive features should fall back to their
     *         unmodified behaviour instead of scaling everything to zero
     */
    [[nodiscard]] virtual bool hasAudio () const { return false; }

    /**
     * Average amplitude of the given 16-band spectrum range (inclusive indices,
     * clamped to [0, 15]), raised to the given exponent
     */
    [[nodiscard]] float getBandAmplitude (int startBand, int endBand, float exponent = 1.0f) const;

    /**
     * Amplitude-driven scale factor for audio-reactive features: interpolates
     * between bounds.x (silence) and bounds.y (loud) using the average amplitude
     * of the given band range raised to the exponent
     *
     * @return 1.0f when there is no real audio data available, so wallpapers
     *         running without audio processing keep behaving as if the feature
     *         was disabled
     */
    [[nodiscard]] float getAudioScale (
	const glm::vec2& bounds, int startBand, int endBand, float exponent = 1.0f
    ) const;

    float audio16[16] = { 0 };
    float audio32[32] = { 0 };
    float audio64[64] = { 0 };
};
} // namespace WallpaperEngine::Audio::Drivers::Recorders
