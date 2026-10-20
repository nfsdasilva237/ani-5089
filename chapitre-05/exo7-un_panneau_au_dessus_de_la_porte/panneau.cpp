// Exo 8 : première salle en cubes. 4 m x 4 m, murs de 2,5 m, origine au centre du sol.
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"
#include "NKFileSystem/NkPath.h"
#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKImage/Core/NkImage.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRHI/Core/NkGraphicsApi.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace nkentseu;
using namespace nkentseu::renderer;

static constexpr bool ORDRE_INVERSE = false;

static float SampleBilinear(const uint8 *pixels, int width, int height,
                            float u, float v) {
    const float x = std::clamp(u * width, 0.0f, float(width - 1));
    const float y = std::clamp(v * height, 0.0f, float(height - 1));
    const int x0 = int(std::floor(x));
    const int y0 = int(std::floor(y));
    const int x1 = std::min(x0 + 1, width - 1);
    const int y1 = std::min(y0 + 1, height - 1);
    const float tx = x - x0;
    const float ty = y - y0;
    const float top = pixels[y0 * width + x0] * (1.0f - tx) +
                      pixels[y0 * width + x1] * tx;
    const float bottom = pixels[y1 * width + x0] * (1.0f - tx) +
                         pixels[y1 * width + x1] * tx;
    return top * (1.0f - ty) + bottom * ty;
}

static bool BuildPanneau(int &imageWidth, int &imageHeight,
                         int &atlasWidth, int &atlasHeight) {
    NkFontAtlas atlas;
    NkFont *font = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
    if (!font || !atlas.Build()) {
        std::cerr << "Impossible de construire l'atlas Inter 32 px.\n";
        return false;
    }

    uint8 *atlasPixels = nullptr;
    int32 bytesPerPixel = 0;
    atlas.GetTexDataAsAlpha8(&atlasPixels, &atlasWidth, &atlasHeight, &bytesPerPixel);
    if (!atlasPixels || atlasWidth <= 0 || atlasHeight <= 0 || bytesPerPixel != 1) {
        std::cerr << "Les pixels Alpha8 de l'atlas sont indisponibles.\n";
        return false;
    }

    const std::string word = "Sortie";
    float textWidth = 0.0f;
    for (char character : word) {
        const NkFontGlyph *glyph = font->FindGlyph(static_cast<NkFontCodepoint>(character));
        if (!glyph) {
            std::cerr << "Glyphe manquant dans l'atlas.\n";
            return false;
        }
        textWidth += glyph->advanceX;
    }

    imageWidth = static_cast<int>(std::ceil(textWidth));
    imageHeight = static_cast<int>(std::ceil(font->ascent - font->descent));
    const int baseline = static_cast<int>(std::ceil(font->ascent));
    NkImage image = NkImage::Create(static_cast<uint32>(imageWidth),
                                    static_cast<uint32>(imageHeight),
                                    static_cast<int32>(4), static_cast<uint32>(0x20334AFF));
    if (!image.IsValid()) {
        std::cerr << "Impossible de creer l'image du panneau.\n";
        return false;
    }

    float cursorX = 0.0f;
    const math::NkColor background(0x20334AFF);
    const math::NkColor foreground(0xFFF4D8FF);
    for (char character : word) {
        const NkFontGlyph *glyph = font->FindGlyph(static_cast<NkFontCodepoint>(character));
        if (glyph->visible && glyph->x1 > glyph->x0 && glyph->y1 > glyph->y0) {
            const float left = cursorX + glyph->x0;
            const float right = cursorX + glyph->x1;
            const float top = baseline + glyph->y0;
            const float bottom = baseline + glyph->y1;
            for (int y = std::max(0, static_cast<int>(std::floor(top)));
                 y < std::min(imageHeight, static_cast<int>(std::ceil(bottom))); ++y) {
                const float fy = std::clamp((y + 0.5f - top) / (bottom - top), 0.0f, 1.0f);
                const float v = glyph->v0 + fy * (glyph->v1 - glyph->v0);
                for (int x = std::max(0, static_cast<int>(std::floor(left)));
                     x < std::min(imageWidth, static_cast<int>(std::ceil(right))); ++x) {
                    const float fx = std::clamp((x + 0.5f - left) / (right - left), 0.0f, 1.0f);
                    const float u = glyph->u0 + fx * (glyph->u1 - glyph->u0);
                    const float coverage = SampleBilinear(atlasPixels, atlasWidth, atlasHeight, u, v) / 255.0f;
                    const auto mix = [coverage](uint8 bg, uint8 fg) {
                        return static_cast<uint8>(std::lround(bg * (1.0f - coverage) + fg * coverage));
                    };
                    image.SetPixel(x, y, math::NkColor(mix(background.r, foreground.r),
                                                       mix(background.g, foreground.g),
                                                       mix(background.b, foreground.b), 255));
                }
            }
        }
        cursorX += glyph->advanceX;
    }

    if (!image.SavePNG("panneau.png")) {
        std::cerr << "Impossible d'enregistrer panneau.png.\n";
        return false;
    }
    return true;
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "Exo7Panneau";
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

    int imageWidth = 0, imageHeight = 0, atlasWidth = 0, atlasHeight = 0;
    if (!BuildPanneau(imageWidth, imageHeight, atlasWidth, atlasHeight)) {
        return 1;
    }
    std::cout << "atlas : " << atlasWidth << " x " << atlasHeight << '\n'
              << "image : " << imageWidth << " x " << imageHeight << '\n';
    {
        std::ofstream report("panneau.txt");
        if (!report) {
            std::cerr << "Impossible d'ouvrir panneau.txt en ecriture.\n";
            return 1;
        }
        report << "atlas : " << atlasWidth << " x " << atlasHeight << '\n'
               << "image : " << imageWidth << " x " << imageHeight << "\n\n"
               << "Ma police et sa taille : Inter embarquee dans le moteur, rasterisee a 32 px.\n\n"
               << "A trois metres : Le panneau mesure 0,45 m de large sur 0,20 m de haut. "
                  "La capture trois-metres.png est prise depuis (-0,70 ; 1,70 ; 1,00).\n\n"
               << "A cinquante centimetres : La meme image de 72 x 32 pixels est posee au-dessus de la porte. "
                  "La capture cinquante-cm.png est prise depuis (-0,70 ; 2,20 ; -1,50).\n";
    }

    NkWindowConfig winCfg;
    winCfg.title = "Exercice 7 - Panneau Sortie";
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
    devInfo.api = NkGraphicsApi::NK_GFX_API_OPENGL;

    NkIDevice *device = NkDeviceFactory::CreateForApi(devInfo.api, devInfo);
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

    const char *relativeTexturePath =
        "chapitre-05/exo4-le_premier_mur_texture/murs/photo.jpg";
    const NkString texturePath = NkPath::LocateResource(relativeTexturePath);
    if (texturePath.Length() == 0) {
        logger.Error("Ressource introuvable : {0}", relativeTexturePath);
    }
    const NkTexHandle texture = texturePath.Length() > 0
                                    ? renderer->GetTextures()->Load(texturePath)
                                    : NkTexHandle{};
    const NkTexHandle signTexture = renderer->GetTextures()->Load("panneau.png");
    NkMaterial *material = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (material && texture.IsValid()) {
        material->SetAlbedoMap(texture);
        material->SetTriplanarTileSize(1.0f);
    }
    NkMaterial *signMaterial = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (signMaterial && signTexture.IsValid()) {
        signMaterial->SetAlbedoMap(signTexture);
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

    const char *viewSetting = std::getenv("PANNEAU_VUE");
    const bool closeView = viewSetting && std::string(viewSetting) == "proche";
    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running)
            break;
        if (!renderer->BeginFrame())
            continue;

        NkCamera3DData camData;
        camData.position = closeView ? NkVec3f{-0.70f, 2.20f, -1.50f}
                                     : NkVec3f{-0.70f, 1.70f, 1.00f};
        camData.target = {-0.70f, 2.12f, -1.93f};
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);

        NkSceneContext sctx;
        sctx.camera = cam;
        r3d->SetIBLColor({1.0f, 0.96f, 0.90f});
        r3d->SetIBLStrength(1.2f);

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 8.f;
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
        r3d->Submit(Boite(cube, {-0.70f, 1.0f, -1.91f}, {0.92f, 2.0f, 0.06f},
                          {0.24f, 0.16f, 0.10f}));
        r3d->Submit(Boite(cube, {-1.19f, 1.02f, -1.90f}, {0.08f, 2.04f, 0.12f},
                          {0.68f, 0.57f, 0.40f}));
        r3d->Submit(Boite(cube, {-0.21f, 1.02f, -1.90f}, {0.08f, 2.04f, 0.12f},
                          {0.68f, 0.57f, 0.40f}));
        r3d->Submit(Boite(cube, {-0.70f, 2.04f, -1.90f}, {1.06f, 0.08f, 0.12f},
                          {0.68f, 0.57f, 0.40f}));
        NkDrawCall3D sign = Boite(cube, {-0.70f, 2.12f, -1.93f},
                                  {0.20f * imageWidth / imageHeight, 0.20f, 0.04f},
                                  {1.f, 1.f, 1.f}, false);
        if (signMaterial && signTexture.IsValid()) {
            sign.material = signMaterial->GetInstHandle();
        }
        r3d->Submit(sign);
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
