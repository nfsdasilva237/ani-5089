#include "NKAudio/NkAudio.h"
#include "NKTime/NkChrono.h"
#include <iostream>

using namespace nkentseu::audio;

int main() {
    AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    if (!AudioEngine::Instance().Initialize(cfg)) return 1;
    std::cout << "dorsal : " << AudioEngine::Instance().GetBackendName() << '\n';
    std::cout << "frequence : " << AudioEngine::Instance().GetSampleRate() << '\n';
    std::cout << "tampon : " << AudioEngine::Instance().GetBufferSize() << '\n';
    std::cout << "latence : " << AudioEngine::Instance().GetLatencyMs() << " ms\n";
    AudioSample son = AudioGenerator::GenerateTone(440.f, 1.f);
    AudioEngine::Instance().Play(son);
    nkentseu::NkChrono::SleepMilliseconds(1200);
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(son);
    return 0;
}
