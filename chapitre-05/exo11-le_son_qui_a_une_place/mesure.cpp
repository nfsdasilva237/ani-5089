#include "NKAudio/NkAudio.h"
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace nkentseu::audio;

struct Rms {
    double left;
    double right;
};

static Rms Measure(AudioSample &rain, AudioEngine &engine, float x, float z, bool positional) {
    VoiceParams p;
    p.looping = true;
    p.source3d.positional = positional;
    p.source3d.position[0] = x;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = z;
    p.source3d.minDistance = 1.f;
    p.source3d.maxDistance = 20.f;
    p.source3d.rolloffFactor = 1.f;
    const AudioHandle voice = engine.Play(rain, p);
    double leftSquares = 0.0, rightSquares = 0.0;
    float block[256 * 2]{};
    for (int rendered = 0; rendered < 48000;) {
        const int frames = (48000 - rendered < 256) ? 48000 - rendered : 256;
        engine.RenderToBuffer(block, frames, 2);
        for (int i = 0; i < frames; ++i) {
            leftSquares += double(block[2 * i]) * block[2 * i];
            rightSquares += double(block[2 * i + 1]) * block[2 * i + 1];
        }
        rendered += frames;
    }
    engine.Stop(voice);
    return {std::sqrt(leftSquares / 48000.0), std::sqrt(rightSquares / 48000.0)};
}

int main() {
    AudioEngineConfig cfg;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    cfg.sampleRate = 48000;
    if (!AudioEngine::Instance().Initialize(cfg)) return 1;
    AudioEngine &engine = AudioEngine::Instance();
    engine.SetListenerPosition(0.f, 0.f, 0.f);
    engine.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);
    AudioSample rain = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);
    const std::array<const char *, 3> labels = {"gauche", "devant", "droite"};
    const std::array<float, 3> xs = {-2.f, 0.f, 2.f};
    const std::array<float, 3> zs = {0.f, -2.f, 0.f};
    std::array<Rms, 6> values{};
    int out = 0;
    for (int position = 0; position < 3; ++position) {
        for (bool positional : {true, false})
            values[out++] = Measure(rain, engine, xs[position], zs[position], positional);
    }
    engine.Shutdown();
    AudioLoader::Free(rain);
    std::cout << std::fixed << std::setprecision(6);
    out = 0;
    for (int position = 0; position < 3; ++position) {
        for (int positional : {1, 0}) {
            const Rms r = values[out++];
            std::cout << labels[position] << ' ' << positional << " : G " << r.left << " x D " << r.right << '\n';
        }
    }
    return 0;
}
