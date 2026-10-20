#include "NKAudio/NkAudio.h"
#include "NKTime/NkChrono.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

using namespace nkentseu::audio;

struct Result {
    const char *model;
    int rolloff;
    float distance;
    float gain;
};

static float Measure(AudioSample &sample, AudioEngine &engine, AttenuationModel model,
                     float distance, float rolloff) {
    VoiceParams p;
    p.looping = true;
    p.source3d.positional = true;
    p.source3d.minDistance = 1.f;
    p.source3d.maxDistance = 100.f;
    p.source3d.rolloffFactor = rolloff;
    p.source3d.attenuationModel = model;
    p.source3d.position[0] = 0.f;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = -distance;
    const AudioHandle voice = engine.Play(sample, p);
    double sumSquares = 0.0;
    long long count = 0;
    float block[256 * 2]{};
    for (int rendered = 0; rendered < 48000;) {
        const int frames = (48000 - rendered < 256) ? 48000 - rendered : 256;
        engine.RenderToBuffer(block, frames, 2);
        for (int i = 0; i < frames; ++i) {
            sumSquares += double(block[2 * i]) * block[2 * i];
            sumSquares += double(block[2 * i + 1]) * block[2 * i + 1];
        }
        rendered += frames;
        count += frames * 2;
    }
    engine.Stop(voice);
    return static_cast<float>(std::sqrt(sumSquares / count));
}

int main() {
    const char *listenMode = std::getenv("ATTEN_ECOUTE");
    if (listenMode) {
        const bool inverse = std::string(listenMode).find("INVERSE") == 0;
        const float distance = std::string(listenMode).find("_8") != std::string::npos ? 8.f : 1.f;
        AudioEngineConfig listenConfig;
        listenConfig.sampleRate = 48000;
        AudioEngine &listenEngine = AudioEngine::Instance();
        if (!listenEngine.Initialize(listenConfig)) return 1;
        AudioSample listenTone = AudioGenerator::GenerateTone(440.f, 2.f, WaveformType::SINE, 48000, 0.1f);
        VoiceParams listenParams;
        listenParams.looping = true;
        listenParams.source3d.positional = true;
        listenParams.source3d.minDistance = 1.f;
        listenParams.source3d.maxDistance = 100.f;
        listenParams.source3d.rolloffFactor = 1.f;
        listenParams.source3d.attenuationModel = inverse ? AttenuationModel::INVERSE : AttenuationModel::LINEAR;
        listenParams.source3d.position[2] = -distance;
        const AudioHandle listenVoice = listenEngine.Play(listenTone, listenParams);
        std::cout << "Ecoute " << (inverse ? "INVERSE" : "LINEAR") << " a " << distance << " m.\n";
        nkentseu::NkChrono::SleepMilliseconds(2200);
        listenEngine.Stop(listenVoice);
        listenEngine.Shutdown();
        AudioLoader::Free(listenTone);
        return 0;
    }
    AudioEngineConfig cfg;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    cfg.sampleRate = 48000;
    if (!AudioEngine::Instance().Initialize(cfg)) return 1;
    AudioEngine &engine = AudioEngine::Instance();
    engine.SetListenerPosition(0.f, 0.f, 0.f);
    engine.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);
    AudioSample tone = AudioGenerator::GenerateTone(440.f, 2.f, WaveformType::SINE, 48000, 0.1f);
    const std::array<AttenuationModel, 4> models = {AttenuationModel::NONE, AttenuationModel::INVERSE,
                                                    AttenuationModel::LINEAR, AttenuationModel::EXPONENTIAL};
    const std::array<const char *, 4> names = {"NONE", "INVERSE", "LINEAR", "EXPONENTIAL"};
    const std::array<float, 5> distances = {0.5f, 1.f, 2.f, 4.f, 8.f};
    std::array<Result, 40> results{};
    int out = 0;
    for (int rolloff : {1, 2}) {
        for (int m = 0; m < 4; ++m) {
            for (float d : distances) {
                const float baseline = Measure(tone, engine, AttenuationModel::NONE, d, float(rolloff));
                const float value = (m == 0) ? baseline : Measure(tone, engine, models[m], d, float(rolloff));
                results[out++] = {names[m], rolloff, d, baseline > 0.f ? value / baseline : 0.f};
            }
        }
    }
    engine.Shutdown();
    AudioLoader::Free(tone);
    for (const Result &r : results)
        std::cout << r.model << ", rolloff " << r.rolloff << ", " << std::defaultfloat << r.distance
                  << " m : " << std::fixed << std::setprecision(3) << r.gain << '\n';
    return 0;
}
