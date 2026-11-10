#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &data) {
    data.appName = "CibleHorsEcran";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static uint64 LireVariable(const char *nom, uint64 valeurParDefaut) {
    const char *texte = std::getenv(nom);
    return texte == nullptr ? valeurParDefaut : std::strtoull(texte, nullptr, 10);
}

int nkmain(const NkEntryState &state) {
    (void)state;
    constexpr uint32 largeurCible = 640;
    constexpr uint32 hauteurCible = 360;

    NkWindowConfig configFenetre;
    configFenetre.title = "Cible hors ecran";
    configFenetre.width = 1280;
    configFenetre.height = 720;
    configFenetre.centered = true;
    configFenetre.vsync = true;
    NkWindow fenetre(configFenetre);
    if (!fenetre.IsValid()) return 1;

    NkDeviceInitInfo info{};
    info.surface = fenetre.GetSurfaceDesc();
    info.width = fenetre.GetSize().x;
    info.height = fenetre.GetSize().y;
    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(info);
    if (device == nullptr) return 1;

    NkRendererConfig configEcran = NkRendererConfig::For2D(info.api, info.width, info.height);
    NkRenderer *ecran = NkRenderer::Create(device, configEcran);
    if (ecran == nullptr || !ecran->Initialize()) {
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkOffscreenDesc description;
    description.width = largeurCible;
    description.height = hauteurCible;
    description.hasDepth = true;
    description.hdr = false;
    description.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
    description.readable = true;
    description.readback = false;
    description.name = "CiblePlusPetiteQueLaFenetre";
    NkOffscreenTarget *cible = ecran->CreateOffscreen(description);
    if (cible == nullptr) {
        NkRenderer::Destroy(ecran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    NkRendererConfig configRendu = NkRendererConfig::ForGame(info.api, info.width, info.height);
    configRendu.Disable(NK_SS_RENDER2D);
    configRendu.Disable(NK_SS_TEXT);
    configRendu.Disable(NK_SS_OVERLAY);
    configRendu.Enable(NK_SS_OFFSCREEN);
    NkRenderer *rendu = NkRenderer::Create(device, configRendu);
    if (rendu == nullptr || !rendu->Initialize()) {
        NkRenderer::Destroy(ecran);
        NkDeviceFactory::Destroy(device);
        return 1;
    }

    rendu->SetFinalColorTarget(ecran->GetTextures()->GetRHIHandle(cible->GetColorResult()));
    const bool correction = LireVariable("MASALLE_CORRIGER", 1) != 0;
    if (correction) rendu->SetRenderSizeOverride(largeurCible, hauteurCible);
    const uint64 limite = LireVariable("MASALLE_EXIT", 0);

    uint64 image = 0;
    bool ouverte = true;
    while (ouverte && fenetre.IsOpen()) {
        NkEvent *evenement = nullptr;
        while (NkEvents().PollEvent(evenement)) {
            if (evenement->As<NkWindowCloseEvent>() != nullptr) ouverte = false;
        }
        if (!ouverte) break;

        if (ecran->BeginFrame()) {
            NkICommandBuffer *commandes = ecran->GetCmd();
            rendu->FlushGraphRebuilds();
            NkRender3D *r3d = rendu->GetRender3D();
            r3d->ResetFrame();
            if (auto *materiaux = rendu->GetMaterialCollection()) materiaux->Upload();

            NkCamera3DData camera;
            camera.position = {0.f, 1.2f, 3.f};
            camera.target = {0.f, 0.8f, 0.f};
            camera.up = {0.f, 1.f, 0.f};
            camera.fovY = 60.f;
            camera.aspect = static_cast<float32>(largeurCible) / hauteurCible;
            camera.nearPlane = 0.05f;
            camera.farPlane = 50.f;
            NkSceneContext scene;
            scene.camera = NkCamera3D(camera);
            scene.time = static_cast<float32>(image) / 60.f;
            r3d->BeginScene(scene);

            NkDrawCall3D cube;
            cube.mesh = rendu->GetMeshSystem()->GetCube();
            cube.transform = NkMat4f::Translate({0.f, 0.8f, 0.f}) * NkMat4f::Scale({0.8f, 0.8f, 0.8f});
            cube.aabb = {{-0.4f, 0.4f, -0.4f}, {0.4f, 1.2f, 0.4f}};
            cube.tint = {0.2f, 0.7f, 0.9f};
            cube.roughness = 0.6f;
            r3d->Submit(cube);

            if (auto *graph = rendu->GetRenderGraph()) graph->Execute(commandes);
            if (auto *r2d = ecran->GetRender2D()) {
                r2d->DrawImage(cible->GetColorResult(), {0.f, 0.f, 1280.f, 720.f});
            }
            ecran->Present();
            ecran->EndFrame();
            ++image;
            std::printf("image : %llu, etat : dessinee\n", static_cast<unsigned long long>(image));
            std::fflush(stdout);
        }

        if (limite != 0 && image >= limite) ouverte = false;
    }

    NkRenderer::Destroy(rendu);
    NkRenderer::Destroy(ecran);
    NkDeviceFactory::Destroy(device);
    return 0;
}
