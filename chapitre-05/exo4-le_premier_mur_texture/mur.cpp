// Exo 8 : première salle en cubes. 4 m x 4 m, murs de 2,5 m, origine au centre du sol.
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
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static constexpr bool ORDRE_INVERSE = false;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Exo4MurTexture";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static NkDrawCall3D Boite(NkMeshHandle mesh, NkVec3f centre, NkVec3f taille,
                          NkVec3f teinte, bool ombre = true) {
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.tint = teinte;
    dc.castShadow = ombre;
    dc.metallic = 0.f;
    dc.roughness = 0.8f;
    if (!ORDRE_INVERSE) {
        dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
        dc.aabb = {centre - taille * 0.5f, centre + taille * 0.5f};
    } else {
        dc.transform = NkMat4f::Scale(taille) * NkMat4f::Translate(centre);
        const NkVec3f c = {centre.x * taille.x, centre.y * taille.y, centre.z * taille.z};
        dc.aabb = {c - taille * 0.5f, c + taille * 0.5f};
    }
    return dc;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exercice 4 - Mur texture";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;
    winCfg.resizable = true;
    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Salle] fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Salle] device KO");
        window.Close();
        return 2;
    }

    const char *apiName = NkGraphicsApiName(device->GetApi());
    logger.Info("Interface graphique : {0}", apiName);

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;

    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Salle] renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    const NkTexHandle texture = renderer->GetTextures()->Load("murs/photo.jpg");
    NkMaterial *material = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (material && texture.IsValid()) {
        material->SetAlbedoMap(texture);
        material->SetTriplanarTileSize(1.0f);
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshSystem *meshes = renderer->GetMeshSystem();
    NkMeshHandle cube = meshes->GetCube();
    NkMeshHandle plan = meshes->GetPlane();

    bool running = true;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            running = false;
    });

    uint32 W = devInfo.width, H = devInfo.height;
    events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e) {
        const uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
        if (w > 0 && h > 0 && (w != W || h != H)) {
            W = w;
            H = h;
            renderer->OnResize(w, h);
        }
    });

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running)
            break;
        if (!renderer->BeginFrame())
            continue;

        NkCamera3DData camData;
        camData.position = {0.f, 1.70f, 1.5f};
        camData.target = {0.f, 1.40f, -2.f};
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);

        NkSceneContext sctx;
        sctx.camera = cam;
        sctx.ambientIntensity = 0.6f;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = true;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        const NkVec3f mur = {0.72f, 0.70f, 0.66f};
        {
            NkDrawCall3D dc;
            dc.mesh = plan;
            dc.transform = NkMat4f::Translate({0.f, 0.f, 0.f}) * NkMat4f::Scale({2.f, 1.f, 2.f});
            dc.aabb = {{-2.f, 0.f, -2.f}, {2.f, 0.f, 2.f}};
            dc.tint = {0.35f, 0.30f, 0.26f};
            dc.castShadow = false;
            r3d->Submit(dc);
        }

        NkDrawCall3D murDuFond = Boite(cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, mur);
        if (material && texture.IsValid()) {
            murDuFond.material = material->GetInstHandle();
        }
        r3d->Submit(murDuFond);
        r3d->Submit(Boite(cube, {0.f, 1.25f,  2.f}, {4.f, 2.5f, 0.1f}, mur));
        r3d->Submit(Boite(cube, {-2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur));
        r3d->Submit(Boite(cube, { 2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur));
        r3d->Submit(Boite(cube, {0.f, 2.55f, 0.f}, {4.1f, 0.1f, 4.1f}, {0.85f, 0.85f, 0.85f}, false));

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
