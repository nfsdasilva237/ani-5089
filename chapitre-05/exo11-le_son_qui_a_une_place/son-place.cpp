#include "NKAudio/NkAudio.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKFileSystem/NkPath.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRHI/Core/NkGraphicsApi.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include <cmath>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::audio;

static void ConfigureApp(NkAppData &data) { data.appName = "Exo11SonPlace"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureApp)

static NkDrawCall3D Box(NkMeshHandle cube, NkVec3f center, NkVec3f size, NkVec3f color) {
    NkDrawCall3D dc;
    dc.mesh = cube;
    dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
    dc.aabb = {center - size * 0.5f, center + size * 0.5f};
    dc.tint = color;
    dc.roughness = 0.85f;
    return dc;
}

int nkmain(const NkEntryState &) {
    NkWindowConfig windowConfig;
    windowConfig.title = "Exercice 11 - pluie placee (Echap pour fermer)";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    NkWindow window(windowConfig);
    if (!window.IsValid()) return 1;
    NkDeviceInitInfo init{};
    init.surface = window.GetSurfaceDesc();
    init.width = uint32(window.GetSize().width);
    init.height = uint32(window.GetSize().height);
    init.api = NkGraphicsApi::NK_GFX_API_OPENGL;
    NkIDevice *device = NkDeviceFactory::CreateForApi(init.api, init);
    if (!device || !device->IsValid()) return 2;
    NkRendererConfig rendererConfig = NkRendererConfig::ForGame(init.api, init.width, init.height);
    rendererConfig.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    NkRenderer *renderer = NkRenderer::Create(device, rendererConfig);
    if (!renderer || !renderer->Initialize()) return 3;

    AudioEngineConfig audioConfig;
    audioConfig.sampleRate = 48000;
    AudioEngine &audio = AudioEngine::Instance();
    if (!audio.Initialize(audioConfig)) return 4;
    AudioSample rain = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);
    VoiceParams voiceParams;
    voiceParams.looping = true;
    voiceParams.source3d.positional = std::getenv("SON_SANS_POSITIONAL") == nullptr;
    voiceParams.source3d.position[0] = -1.98f;
    voiceParams.source3d.position[1] = 1.40f;
    voiceParams.source3d.position[2] = -0.90f;
    voiceParams.source3d.minDistance = 0.5f;
    voiceParams.source3d.maxDistance = 12.f;
    voiceParams.source3d.rolloffFactor = 1.f;

    const char *view = std::getenv("SON_VUE");
    NkVec3f cameraPosition = {-0.70f, 1.70f, 0.00f};
    if (view && view[0] == '1') cameraPosition = {-1.75f, 1.45f, -0.35f};
    if (view && view[0] == '3') cameraPosition = {1.55f, 1.70f, 1.45f};
    const NkVec3f lookTarget = {-0.30f, 1.40f, -1.10f};
    NkVec3f forward = lookTarget - cameraPosition;
    const float length = std::sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
    forward = forward / length;
    audio.SetListenerPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    audio.SetListenerOrientation(forward.x, forward.y, forward.z, 0.f, 1.f, 0.f);
    const AudioHandle rainVoice = audio.Play(rain, voiceParams);

    NkTexHandle texture = renderer->GetTextures()->Load(
        NkPath::LocateResource("chapitre-05/exo4-le_premier_mur_texture/murs/photo.jpg"));
    NkMaterial *material = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (material && texture.IsValid()) material->SetAlbedoMap(texture);
    NkRender3D *scene = renderer->GetRender3D();
    scene->SetIBLColor({1.0f, 0.96f, 0.90f});
    scene->SetIBLStrength(1.2f);
    NkMeshHandle cube = renderer->GetMeshSystem()->GetCube();
    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *event) {
        if (event->GetKey() == NkKey::NK_ESCAPE) running = false;
    });
    uint32 width = init.width, height = init.height;
    events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *event) {
        width = uint32(event->GetWidth());
        height = uint32(event->GetHeight());
        if (width && height) renderer->OnResize(width, height);
    });
    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!renderer->BeginFrame()) continue;
        NkCamera3DData cameraData;
        cameraData.position = cameraPosition;
        cameraData.target = lookTarget;
        cameraData.up = {0.f, 1.f, 0.f};
        cameraData.fovY = 70.f;
        cameraData.aspect = float(width) / float(height);
        cameraData.nearPlane = 0.05f;
        cameraData.farPlane = 200.f;
        NkSceneContext context;
        context.camera = NkCamera3D(cameraData);
        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.98f, 0.92f};
        sun.intensity = 8.f;
        context.lights.PushBack(sun);
        scene->BeginScene(context);
        NkDrawCall3D floor;
        floor.mesh = renderer->GetMeshSystem()->GetPlane();
        floor.transform = NkMat4f::Translate({0.f, 0.f, 0.f}) * NkMat4f::Scale({2.f, 1.f, 2.f});
        floor.aabb = {{-2.f, 0.f, -2.f}, {2.f, 0.f, 2.f}};
        floor.tint = {0.35f, 0.30f, 0.26f};
        floor.castShadow = false;
        scene->Submit(floor);
        NkDrawCall3D backWall = Box(cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, {0.72f, 0.70f, 0.66f});
        if (material && texture.IsValid()) backWall.material = material->GetInstHandle();
        scene->Submit(backWall);
        scene->Submit(Box(cube, {0.f, 1.25f, 2.f}, {4.f, 2.5f, 0.1f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 1.25f, 1.10f}, {0.1f, 2.5f, 1.80f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 1.25f, -1.70f}, {0.1f, 2.5f, 0.60f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 0.50f, -0.90f}, {0.1f, 1.00f, 1.20f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 2.175f, -0.90f}, {0.1f, 0.65f, 1.20f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 1.45f, -1.50f}, {0.1f, 0.80f, 0.08f}, {0.25f, 0.22f, 0.18f}));
        scene->Submit(Box(cube, {-2.f, 1.45f, -0.30f}, {0.1f, 0.80f, 0.08f}, {0.25f, 0.22f, 0.18f}));
        scene->Submit(Box(cube, {-1.95f, 1.45f, -0.90f}, {0.04f, 0.80f, 1.12f}, {0.35f, 0.55f, 0.68f}));
        scene->Submit(Box(cube, {2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {0.f, 2.55f, 0.f}, {4.1f, 0.1f, 4.1f}, {0.85f, 0.85f, 0.85f}));
        renderer->Present();
        renderer->EndFrame();
    }
    audio.Stop(rainVoice);
    audio.Shutdown();
    AudioLoader::Free(rain);
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
