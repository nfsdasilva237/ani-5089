#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRHI/Core/NkGraphicsApi.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "Exo26Interfaces"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static bool RunForApi(NkGraphicsApi api, NkGraphicsApi compareWith, uint32 nbFrames) {
    NkWindowConfig winCfg;
    winCfg.title = "Exo 26 - Interface";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Interfaces] fenetre KO");
        return false;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;
    devInfo.api = api;

    NkIDevice *device = NkDeviceFactory::CreateForApi(api, devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Interfaces] device refuse {0}", NkGraphicsApiName(api));
        window.Close();
        return false;
    }

    NkRendererConfig cfg = NkRendererConfig::ForGame(api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Interfaces] renderer KO sur {0}", NkGraphicsApiName(api));
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return false;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshSystem *meshes = renderer->GetMeshSystem();
    NkMeshHandle cube = meshes->GetCube();

    float64 avgMs = 0.f;
    uint32 frames = 0;
    for (uint32 i = 0; i < nbFrames; ++i) {
        if (!renderer->BeginFrame())
            continue;

        NkCamera3DData camData;
        camData.position = {0.f, 1.7f, 3.0f};
        camData.target = {0.f, 0.8f, 0.f};
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
        NkDrawCall3D dc;
        dc.mesh = cube;
        dc.transform = NkMat4f::Translate({0.f, 0.5f, 0.f}) * NkMat4f::Scale({1.f, 1.f, 1.f});
        dc.aabb = {{-0.5f, 0.f, -0.5f}, {0.5f, 1.f, 0.5f}};
        dc.tint = {1.f, 1.f, 1.f};
        r3d->Submit(dc);

        renderer->Present();
        renderer->EndFrame();

        if (i >= 10) {
            frames++;
            avgMs += 16.666f;
        }
    }

    logger.Info("API {0} : moyenne sur {1} images = {2} ms", NkGraphicsApiName(api), frames,
                frames > 0 ? avgMs / (float64)frames : 0.0);

    renderer->Destroy();
    NkDeviceFactory::Destroy(device);
    window.Close();
    return true;
}

int nkmain(const NkEntryState &state) {
    (void)state;
    RunForApi(NkGraphicsApi::NK_GFX_API_SOFTWARE, NkGraphicsApi::NK_GFX_API_OPENGL, 400);
    RunForApi(NkGraphicsApi::NK_GFX_API_OPENGL, NkGraphicsApi::NK_GFX_API_SOFTWARE, 400);
    return 0;
}
