// Compare une police Inter ordinaire et la meme police rasterisee en SDF.
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace nkentseu;
using namespace nkentseu::renderer;

static float clamp01(float x) { return std::clamp(x, 0.0f, 1.0f); }

static float smoothstep(float a, float b, float x) {
    const float t = clamp01((x - a) / (b - a));
    return t * t * (3.0f - 2.0f * t);
}

static float SampleBilinear(const uint8 *pixels, int width, int height, float u, float v) {
    const float x = std::clamp(u * width, 0.0f, float(width - 1));
    const float y = std::clamp(v * height, 0.0f, float(height - 1));
    const int x0 = int(std::floor(x)), y0 = int(std::floor(y));
    const int x1 = std::min(x0 + 1, width - 1), y1 = std::min(y0 + 1, height - 1);
    const float tx = x - x0, ty = y - y0;
    const float top = pixels[y0 * width + x0] * (1.0f - tx) + pixels[y0 * width + x1] * tx;
    const float bottom = pixels[y1 * width + x0] * (1.0f - tx) + pixels[y1 * width + x1] * tx;
    return top * (1.0f - ty) + bottom * ty;
}

static NkFont *BuildFont(NkFontAtlas &atlas, float size, bool sdf) {
    atlas.sdfMode = sdf;
    atlas.sdfSpread = 6;
    NkFont *font = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, size);
    if (!font || !atlas.Build()) return nullptr;
    return font;
}

static bool AtlasSize(float size, bool sdf, int &width, int &height) {
    NkFontAtlas atlas;
    if (!BuildFont(atlas, size, sdf)) return false;
    uint8 *pixels = nullptr;
    int32 bpp = 0;
    atlas.GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
    return pixels && width > 0 && height > 0 && bpp == 1;
}

static bool MakePanel(bool sdf, const std::string &filePath) {
    constexpr float scale = 8.0f;
    constexpr float edgeSoftness = 0.08f;
    NkFontAtlas atlas;
    NkFont *font = BuildFont(atlas, 32.0f, sdf);
    if (!font) return false;

    uint8 *atlasPixels = nullptr;
    int atlasWidth = 0, atlasHeight = 0, bytesPerPixel = 0;
    atlas.GetTexDataAsAlpha8(&atlasPixels, &atlasWidth, &atlasHeight, &bytesPerPixel);
    if (!atlasPixels || bytesPerPixel != 1) return false;

    const std::string word = "Sortie";
    float textWidth = 0.0f;
    for (char c : word) {
        const NkFontGlyph *glyph = font->FindGlyph(static_cast<NkFontCodepoint>(c));
        if (!glyph) return false;
        textWidth += glyph->advanceX;
    }
    const int imageWidth = int(std::ceil(textWidth * scale));
    const int imageHeight = int(std::ceil((font->ascent - font->descent) * scale));
    const float baseline = font->ascent * scale;
    NkImage image = NkImage::Create(uint32(imageWidth), uint32(imageHeight), int32(4), uint32(0x20334AFF));
    if (!image.IsValid()) return false;

    const math::NkColor background(0x20334AFF);
    const math::NkColor foreground(0xFFF4D8FF);
    float cursor = 0.0f;
    for (char c : word) {
        const NkFontGlyph *glyph = font->FindGlyph(static_cast<NkFontCodepoint>(c));
        if (glyph->visible && glyph->x1 > glyph->x0 && glyph->y1 > glyph->y0) {
            const float left = (cursor + glyph->x0) * scale;
            const float right = (cursor + glyph->x1) * scale;
            const float top = baseline + glyph->y0 * scale;
            const float bottom = baseline + glyph->y1 * scale;
            for (int y = std::max(0, int(std::floor(top)));
                 y < std::min(imageHeight, int(std::ceil(bottom))); ++y) {
                const float fy = clamp01((y + 0.5f - top) / (bottom - top));
                const float v = glyph->v0 + fy * (glyph->v1 - glyph->v0);
                for (int x = std::max(0, int(std::floor(left)));
                     x < std::min(imageWidth, int(std::ceil(right))); ++x) {
                    const float fx = clamp01((x + 0.5f - left) / (right - left));
                    const float u = glyph->u0 + fx * (glyph->u1 - glyph->u0);
                    const float value = SampleBilinear(atlasPixels, atlasWidth, atlasHeight, u, v) / 255.0f;
                    const float alpha = sdf ? smoothstep(0.5f - edgeSoftness, 0.5f + edgeSoftness, value)
                                            : value;
                    const auto blend = [alpha](uint8 bg, uint8 fg) {
                        return uint8(std::lround(bg * (1.0f - alpha) + fg * alpha));
                    };
                    image.SetPixel(x, y, math::NkColor(blend(background.r, foreground.r),
                                                       blend(background.g, foreground.g),
                                                       blend(background.b, foreground.b), 255));
                }
            }
        }
        cursor += glyph->advanceX;
    }
    image.FlipHorizontal();
    return image.SavePNG(filePath.c_str());
}

static NkDrawCall3D Box(NkMeshHandle mesh, NkVec3f center, NkVec3f size, NkVec3f tint) {
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.transform = NkMat4f::Translate(center) * NkMat4f::Scale(size);
    dc.aabb = {center - size * 0.5f, center + size * 0.5f};
    dc.tint = tint;
    dc.roughness = 0.85f;
    return dc;
}

static void ConfigureApp(NkAppData &data) { data.appName = "Exo8SDF"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureApp)

int nkmain(const NkEntryState &) {
    const std::filesystem::path outputDir = "Build/SdfAssets";
    std::filesystem::create_directories(outputDir);
    const std::string ordinaryPath = (outputDir / "panneau-ordinaire.png").string();
    const std::string sdfPath = (outputDir / "panneau-sdf.png").string();

    int ordinary32W = 0, ordinary32H = 0, sdf32W = 0, sdf32H = 0;
    int ordinary128W = 0, ordinary128H = 0;
    if (!AtlasSize(32.0f, false, ordinary32W, ordinary32H) ||
        !AtlasSize(32.0f, true, sdf32W, sdf32H) ||
        !AtlasSize(128.0f, false, ordinary128W, ordinary128H) ||
        !MakePanel(false, ordinaryPath) || !MakePanel(true, sdfPath)) {
        std::cerr << "Erreur lors de la creation des atlas ou des panneaux.\n";
        return 1;
    }

    {
        std::ofstream report("chapitre-05/exo8-le_sdf_mesure/atlas.txt");
        if (!report) return 1;
        report << "ordinaire 32 : " << ordinary32W << " x " << ordinary32H << '\n'
               << "sdf 32 : " << sdf32W << " x " << sdf32H << '\n'
               << "ordinaire 128 : " << ordinary128W << " x " << ordinary128H << '\n';
    }
    if (std::getenv("SDF_METRIQUES_ONLY")) return 0;

    const char *methodEnv = std::getenv("SDF_METHODE");
    const bool useSdf = methodEnv && std::string(methodEnv) == "sdf";
    const char *viewEnv = std::getenv("SDF_VUE");
    const bool closeView = viewEnv && std::string(viewEnv) == "proche";
    const std::string &chosenPath = useSdf ? sdfPath : ordinaryPath;

    NkWindowConfig windowConfig;
    windowConfig.title = useSdf ? "Exercice 8 - panneau SDF" : "Exercice 8 - panneau ordinaire";
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.centered = true;
    NkWindow window(windowConfig);
    if (!window.IsValid()) return 2;

    NkDeviceInitInfo init{};
    init.surface = window.GetSurfaceDesc();
    init.width = uint32(window.GetSize().width);
    init.height = uint32(window.GetSize().height);
    init.api = NkGraphicsApi::NK_GFX_API_OPENGL;
    NkIDevice *device = NkDeviceFactory::CreateForApi(init.api, init);
    if (!device || !device->IsValid()) return 3;
    NkRendererConfig rendererConfig = NkRendererConfig::ForGame(init.api, init.width, init.height);
    rendererConfig.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    NkRenderer *renderer = NkRenderer::Create(device, rendererConfig);
    if (!renderer || !renderer->Initialize()) return 4;
    NkRender3D *scene = renderer->GetRender3D();
    scene->SetIBLColor({1.0f, 0.96f, 0.90f});
    scene->SetIBLStrength(1.2f);

    const NkTexHandle panelTexture = renderer->GetTextures()->Load(NkString(chosenPath.c_str()));
    NkMaterial *panelMaterial = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    if (panelMaterial && panelTexture.IsValid()) panelMaterial->SetAlbedoMap(panelTexture);
    NkMeshSystem *meshes = renderer->GetMeshSystem();
    const NkMeshHandle cube = meshes->GetCube();
    const NkMeshHandle plane = meshes->GetPlane();

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
        NkCamera3DData camera;
        camera.position = closeView ? NkVec3f{-0.70f, 2.20f, -1.50f}
                                    : NkVec3f{-0.70f, 1.70f, 1.00f};
        camera.target = {-0.70f, 2.12f, -1.93f};
        camera.up = {0.f, 1.f, 0.f};
        camera.fovY = 70.f;
        camera.aspect = float(width) / float(height);
        camera.nearPlane = 0.05f;
        camera.farPlane = 200.f;

        NkSceneContext context;
        context.camera = NkCamera3D(camera);
        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.98f, 0.92f};
        sun.intensity = 8.f;
        context.lights.PushBack(sun);
        scene->BeginScene(context);

        NkDrawCall3D floor;
        floor.mesh = plane;
        floor.transform = NkMat4f::Translate({0.f, 0.f, 0.f}) * NkMat4f::Scale({2.f, 1.f, 2.f});
        floor.aabb = {{-2.f, 0.f, -2.f}, {2.f, 0.f, 2.f}};
        floor.tint = {0.45f, 0.40f, 0.34f};
        floor.castShadow = false;
        scene->Submit(floor);
        scene->Submit(Box(cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {0.f, 1.25f, 2.f}, {4.f, 2.5f, 0.1f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {-2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, {0.72f, 0.70f, 0.66f}));
        scene->Submit(Box(cube, {0.f, 2.55f, 0.f}, {4.1f, 0.1f, 4.1f}, {0.85f, 0.85f, 0.85f}));

        scene->Submit(Box(cube, {-0.70f, 1.0f, -1.91f}, {0.92f, 2.0f, 0.06f}, {0.24f, 0.16f, 0.10f}));
        scene->Submit(Box(cube, {-1.19f, 1.02f, -1.90f}, {0.08f, 2.04f, 0.12f}, {0.68f, 0.57f, 0.40f}));
        scene->Submit(Box(cube, {-0.21f, 1.02f, -1.90f}, {0.08f, 2.04f, 0.12f}, {0.68f, 0.57f, 0.40f}));
        scene->Submit(Box(cube, {-0.70f, 2.04f, -1.90f}, {1.06f, 0.08f, 0.12f}, {0.68f, 0.57f, 0.40f}));
        NkDrawCall3D panel = Box(cube, {-0.70f, 2.12f, -1.93f}, {0.45f, 0.20f, 0.04f}, {1.f, 1.f, 1.f});
        if (panelMaterial && panelTexture.IsValid()) panel.material = panelMaterial->GetInstHandle();
        scene->Submit(panel);
        renderer->Present();
        renderer->EndFrame();
    }

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
