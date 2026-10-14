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

static void ConfigureAppData(NkAppData &d) { d.appName = "Exo25Lumiere"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static NkDrawCall3D Boite(NkMeshHandle mesh, NkVec3f centre, NkVec3f taille, NkVec3f teinte) {
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
    dc.aabb = {centre - taille * 0.5f, centre + taille * 0.5f};
    dc.tint = teinte;
    dc.castShadow = true;
    dc.metallic = 0.f;
    dc.roughness = 0.8f;
    return dc;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo 25 - Lumière";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Lumiere] fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Lumiere] device KO");
        window.Close();
        return 2;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Lumiere] renderer KO");
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

    int preset = 0;
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_KEY_1) preset = 0;
        if (e->GetKey() == NkKey::NK_KEY_2) preset = 1;
        if (e->GetKey() == NkKey::NK_KEY_3) preset = 2;
        if (e->GetKey() == NkKey::NK_KEY_4) preset = 3;
    });

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running) break;
        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.position = {0.f, 1.7f, 3.5f};
        camData.target = {0.f, 1.2f, 0.f};
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 60.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 50.f;
        NkCamera3D cam(camData);

        NkSceneContext sctx;
        sctx.camera = cam;
        sctx.ambientIntensity = 0.25f;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = true;

        switch (preset) {
        case 0:
            sun.direction = {-0.4f, -1.f, -0.3f};
            sun.intensity = 3.f;
            break;
        case 1:
            sun.direction = {-0.9f, -1.f, -0.2f};
            sun.intensity = 3.f;
            break;
        case 2:
            sun.direction = {-0.4f, -1.f, -0.3f};
            sun.intensity = 1.5f;
            break;
        case 3:
            sun.direction = {-0.2f, -1.f, -0.8f};
            sun.intensity = 5.f;
            break;
        default:
            sun.direction = {-0.4f, -1.f, -0.3f};
            sun.intensity = 3.f;
            break;
        }

        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        NkDrawCall3D sol;
        sol.mesh = plane;
        sol.transform = NkMat4f::Scale({12.f, 1.f, 12.f});
        sol.aabb = {{-12.f, 0.f, -12.f}, {12.f, 0.f, 12.f}};
        sol.tint = {0.22f, 0.23f, 0.25f};
        sol.castShadow = false;
        r3d->Submit(sol);

        r3d->Submit(Boite(cube, {0.f, 0.5f, 0.f}, {1.f, 1.f, 1.f}, {0.8f, 0.8f, 0.8f}));

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
