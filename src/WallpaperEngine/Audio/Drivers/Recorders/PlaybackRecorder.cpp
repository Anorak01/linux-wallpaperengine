#include "PlaybackRecorder.h"

#include <algorithm>
#include <cmath>
#include <glm/common.hpp>

namespace WallpaperEngine::Audio::Drivers::Recorders {
void PlaybackRecorder::update () { }

float PlaybackRecorder::getBandAmplitude (const int startBand, const int endBand, const float exponent) const {
    const int start = std::clamp (startBand, 0, 15);
    const int end = std::clamp (endBand, start, 15);

    float amplitude = 0.0f;

    for (int band = start; band <= end; band++) {
	amplitude += this->audio16[band];
    }

    amplitude /= static_cast<float> (end - start + 1);

    if (exponent == 1.0f) {
	return amplitude;
    }

    return std::pow (amplitude, exponent);
}

float PlaybackRecorder::getAudioScale (
    const glm::vec2& bounds, const int startBand, const int endBand, const float exponent
) const {
    // without real audio data everything should behave like the feature was disabled
    if (!this->hasAudio ()) {
	return 1.0f;
    }

    const float amplitude = this->getBandAmplitude (startBand, endBand, exponent);

    return bounds.x + (bounds.y - bounds.x) * amplitude;
}

} // namespace WallpaperEngine::Audio::Drivers::Recorders
