#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "Exo21Cube"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static NkDrawCall3D Cube(NkMeshHandle mesh, NkVec3f centre, NkVec3f taille, NkVec3f tint) {
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
    dc.aabb = {centre - taille * 0.5f, centre + taille * 0.5f};
    dc.tint = tint;
    dc.castShadow = true;
    dc.metallic = 0.f;
    dc.roughness = 0.8f;
    return dc;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo 21 - Cube";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Cube] fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Cube] device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;

    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Cube] renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshSystem *meshes = renderer->GetMeshSystem();
    NkMeshHandle cube = meshes->GetCube();
    NkMeshHandle plane = meshes->GetPlane();

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            running = false;
    });

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.position = {0.f, 1.7f, 3.5f};
        camData.target = {0.f, 0.5f, 0.f};
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 60.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);

        NkSceneContext sctx;
        sctx.camera = cam;
        sctx.ambientIntensity = 0.2f;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        NkDrawCall3D ground;
        ground.mesh = plane;
        ground.transform = NkMat4f::Scale({6.f, 1.f, 6.f});
        ground.aabb = {{-6.f, 0.f, -6.f}, {6.f, 0.f, 6.f}};
        ground.tint = {0.2f, 0.2f, 0.2f};
        ground.castShadow = false;
        r3d->Submit(ground);

        r3d->Submit(Cube(cube, {0.f, 0.5f, 0.f}, {1.f, 1.f, 1.f}, {0.9f, 0.9f, 0.9f}));
        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
