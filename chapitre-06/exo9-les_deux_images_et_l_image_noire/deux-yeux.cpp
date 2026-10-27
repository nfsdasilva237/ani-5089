// =============================================================================
//  Étape 5 — la salle, en cubes, aux dimensions du chapitre 2.
//
//  Pas encore de casque, pas encore de tête : une caméra fixe, posée à hauteur
//  d'yeux, qui regarde la porte. C'est le décor sur lequel tout le reste du
//  livre se jugera, et c'est pour cela qu'il est bâti maintenant.
//
//  Toutes les dimensions sont en MÈTRES, et viennent du tableau du chapitre 7 :
//      sol 4 x 4, murs 2,5 de haut, porte 0,9 x 2,0,
//      deux fenêtres 1,2 x 1,0, table 1,2 x 0,8 avec le plateau à 0,75.
//
//  Le cube et la sphère de NkMeshSystem sont UNITAIRES, de demi-étendue 0,5 :
//  une échelle S donne donc une taille S, et poser un objet au sol demande de
//  le translater de S/2. Le plan de GetPlane() est unitaire LUI AUSSI, de
//  demi-étendue 0,5 : une échelle S y donne un côté S, et non un demi-côté.
//  Se tromper là-dessus donne un sol deux fois trop petit, qui flotte au
//  milieu de la pièce, et rien ne le signale.
// =============================================================================
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
#include "NKImage/Core/NkImage.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <string>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	// ── Les dimensions de la salle, en un seul endroit ────────────────────
	// Elles sont ici, nommées, et pas semées dans le code : au chapitre 21 on
	// les lira depuis un fichier, et ce sont ces noms-là qui deviendront des
	// champs.
	const float32 kPiece = 4.00f;	  // côté intérieur du sol
	const float32 kPlafond = 2.50f;	  // hauteur sous plafond
	const float32 kMur = 0.10f;		  // épaisseur d'un mur
	const float32 kPorteL = 0.90f;	  // porte : largeur
	const float32 kPorteH = 2.00f;	  //         hauteur
	const float32 kFenL = 1.20f;	  // fenêtre : largeur
	const float32 kFenH = 1.00f;	  //           hauteur
	const float32 kFenY = 1.40f;	  //           hauteur de son centre
	const float32 kTableL = 1.20f;	  // table : longueur
	const float32 kTableP = 0.80f;	  //         profondeur
	const float32 kTableH = 0.75f;	  //         hauteur du plateau
	const float32 kYeux = 1.70f;	  // hauteur des yeux, debout

	const float32 kDemi = kPiece * 0.5f;

	// Un pavé posé dans la salle. `centre` est le centre géométrique, `taille`
	// les trois dimensions en mètres : on ne manipule jamais autre chose.
	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f) {
		NkDrawCall3D dc;
		dc.mesh = meshes->GetCube();
		dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
		dc.aabb = { { centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f,
					  centre.z - taille.z * 0.5f },
					{ centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f,
					  centre.z + taille.z * 0.5f } };
		dc.tint = teinte;
		dc.metallic = 0.f;
		dc.roughness = rugosite;
		r3d->Submit(dc);
	}

	void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes) {
		const NkVec3f grisMur = { 0.72f, 0.70f, 0.66f };
		const NkVec3f grisSol = { 0.38f, 0.36f, 0.34f };
		const NkVec3f bois = { 0.55f, 0.38f, 0.22f };
		const NkVec3f vitre = { 0.75f, 0.85f, 0.92f };

		{ // le sol : le plan est unitaire, une échelle S donne un côté S
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Scale({ kPiece, 1.f, kPiece });
			dc.aabb = { { -kDemi, 0.f, -kDemi }, { kDemi, 0.f, kDemi } };
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		// Quatre murs. La porte est dans celui du fond, les fenêtres à gauche.
		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		// Le plafond. On l'oublie toujours, et le trou ne se voit qu'en levant
		// les yeux, c'est-à-dire le jour de la démonstration.
		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);

		// La porte, fermée jusqu'au chapitre 17 : un panneau plaqué contre le
		// mur du fond, décalé pour ne pas se battre avec lui en profondeur.
		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

		// Deux fenêtres sur le mur de gauche, pour qu'il y ait un dehors.
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);

		// La table : un plateau et quatre pieds. Le chapitre 12 y posera un
		// objet, le 21 la déplacera.
		const float32 tx = 0.60f, tz = 0.70f;
		const float32 ep = 0.05f;
		Pave(r3d, meshes, { tx, kTableH - ep * 0.5f, tz },
			 { kTableL, ep, kTableP }, bois, 0.5f);

		const float32 pied = 0.06f;
		const float32 hp = kTableH - ep;
		for (int i = 0; i < 4; ++i) {
			const float32 dx = ((i & 1) ? 1.f : -1.f) * (kTableL * 0.5f - pied);
			const float32 dz = ((i & 2) ? 1.f : -1.f) * (kTableP * 0.5f - pied);
			Pave(r3d, meshes, { tx + dx, hp * 0.5f, tz + dz },
				 { pied, hp, pied }, bois, 0.6f);
		}

		// Le repère d'un mètre du chapitre 9 : un cube d'un mètre de côté,
		// posé au sol, contre lequel on juge l'échelle de tout le reste.
		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

} // namespace

struct DifferenceStats { unsigned pixels = 0; unsigned maximum = 0; };

static DifferenceStats Soustraire(NkImage &left, NkImage &right, NkImage &diff) {
	DifferenceStats stats;
	for (unsigned y = 0; y < 720; ++y) {
		for (unsigned x = 0; x < 640; ++x) {
			const auto a = left.GetPixel(x, y);
			const auto b = right.GetPixel(x, y);
			const unsigned delta = std::max({
				(unsigned)std::abs((int)a.r - (int)b.r),
				(unsigned)std::abs((int)a.g - (int)b.g),
				(unsigned)std::abs((int)a.b - (int)b.b)});
			if (delta > 1) ++stats.pixels;
			stats.maximum = std::max(stats.maximum, delta);
			const uint8 value = (uint8)std::min(255u, delta * 4u);
			diff.SetPixel(x, y, math::NkColor(value, value, value, 255));
		}
	}
	return stats;
}

int nkmain(const NkEntryState &state) {
	(void)state;
	NkWindowConfig wc;
	wc.title = "Deux yeux, deux rendus";
	wc.width = 1280;
	wc.height = 720;
	wc.centered = true;
	NkWindow window(wc);
	if (!window.IsValid()) return 1;

	NkDeviceInitInfo init{};
	init.surface = window.GetSurfaceDesc();
	init.width = 1280;
	init.height = 720;
	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(init);
	if (!device) return 1;

	NkRendererConfig screenCfg = NkRendererConfig::For2D(init.api, 1280, 720);
	NkRenderer *screen = NkRenderer::Create(device, screenCfg);
	if (!screen || !screen->Initialize()) return 1;

	NkOffscreenDesc desc{};
	desc.width = 640;
	desc.height = 720;
	desc.hasDepth = true;
	desc.hdr = false;
	desc.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	desc.readable = true;
	desc.readback = true;
	desc.name = "EyeTarget";
	NkOffscreenTarget *targetLeft = screen->CreateOffscreen(desc);
	desc.name = "RightTarget";
	NkOffscreenTarget *targetRight = screen->CreateOffscreen(desc);
	if (!targetLeft || !targetRight) return 1;

	NkRendererConfig eyeCfg = NkRendererConfig::ForGame(init.api, 640, 720);
	eyeCfg.Disable(NK_SS_RENDER2D);
	eyeCfg.Disable(NK_SS_TEXT);
	eyeCfg.Disable(NK_SS_OVERLAY);
	eyeCfg.Enable(NK_SS_OFFSCREEN);
	if (EnvU64("MASALLE_POST", 0) == 0) eyeCfg.Disable(NK_SS_POST_PROCESS);
	NkRenderer *left = NkRenderer::Create(device, eyeCfg);
	NkRenderer *right = NkRenderer::Create(device, eyeCfg);
	if (!left || !left->Initialize() || !right || !right->Initialize()) return 1;
	left->SetFinalColorTarget(screen->GetTextures()->GetRHIHandle(targetLeft->GetColorResult()));
	right->SetFinalColorTarget(screen->GetTextures()->GetRHIHandle(targetRight->GetColorResult()));
	left->SetRenderSizeOverride(640, 720);
	right->SetRenderSizeOverride(640, 720);

	const char *dir = std::getenv("MASALLE_OUTPUT_DIR");
	if (!dir) dir = ".";
	auto path = [dir](const char *name) {
		std::string p(dir);
		if (!p.empty() && p.back() != '/' && p.back() != '\\') p += '\\';
		p += name;
		return p;
	};
	const NkVec3f head(0.30f, 1.70f, 1.20f);
	const float halfIpd = 0.063f * 0.5f;
	uint64 frame = 0;
	bool running = true;
	while (running && window.IsOpen() && frame < 60) {
		NkEvent *event = nullptr;
		while (NkEvents().PollEvent(event)) {
			if (event->As<NkWindowCloseEvent>()) running = false;
		}
		if (!running) break;
		if (!screen->BeginFrame()) continue;
		NkICommandBuffer *cmd = screen->GetCmd();
		const bool noShift = frame >= 30;
		const NkVec3f eyeL = noShift ? head : head + NkVec3f(-halfIpd, 0.f, 0.f);
		const NkVec3f eyeR = noShift ? head : head + NkVec3f(halfIpd, 0.f, 0.f);
		NkRenderer *eyes[] = {left, right};
		NkVec3f positions[] = {eyeL, eyeR};
		for (int i = 0; i < 2; ++i) {
			NkRenderer *renderer = eyes[i];
			NkRender3D *r3d = renderer->GetRender3D();
			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *materials = renderer->GetMaterialCollection()) materials->Upload();
			NkCamera3DData camera;
			camera.position = positions[i];
			camera.target = positions[i] + NkVec3f(0.f, 0.f, -1.f);
			camera.up = NkVec3f(0.f, 1.f, 0.f);
			camera.fovY = 70.f;
			camera.aspect = 640.f / 720.f;
			camera.nearPlane = 0.05f;
			camera.farPlane = 100.f;
			NkSceneContext scene;
			scene.camera = NkCamera3D(camera);
			scene.time = (float32)frame / 60.f;
			r3d->SetIBLStrength(0.35f);
			NkLightDesc light;
			light.type = NkLightType::NK_DIRECTIONAL;
			light.direction = {-0.5f, -1.f, -0.35f};
			light.color = {1.f, 0.96f, 0.88f};
			light.intensity = 3.f;
			light.castShadow = false;
			scene.lights.PushBack(light);
			r3d->BeginScene(scene);
			SoumettreLaSalle(r3d, renderer->GetMeshSystem());
			if (auto *graph = renderer->GetRenderGraph()) graph->Execute(cmd);
		}
		if (auto *r2d = screen->GetRender2D()) {
			r2d->DrawImage(targetLeft->GetColorResult(), {0.f, 0.f, 640.f, 720.f});
			r2d->DrawImage(targetRight->GetColorResult(), {640.f, 0.f, 640.f, 720.f});
		}
		screen->Present();
		screen->EndFrame();
		++frame;
		if (frame == 30) {
			targetLeft->Capture(path("oeil-gauche.png").c_str());
			targetRight->Capture(path("oeil-droit.png").c_str());
		}
		if (frame == 60) {
			targetLeft->Capture(path("oeil-gauche-sans-decalage.png").c_str());
			targetRight->Capture(path("oeil-droit-sans-decalage.png").c_str());
			running = false;
		}
	}

	NkRenderer::Destroy(right);
	NkRenderer::Destroy(left);
	NkRenderer::Destroy(screen);
	NkDeviceFactory::Destroy(device);

	NkImage leftImage, rightImage, leftSame, rightSame;
	if (!leftImage.Load(path("oeil-gauche.png").c_str()) ||
		!rightImage.Load(path("oeil-droit.png").c_str()) ||
		!leftSame.Load(path("oeil-gauche-sans-decalage.png").c_str()) ||
		!rightSame.Load(path("oeil-droit-sans-decalage.png").c_str())) return 1;
	NkImage diff = NkImage::Create((uint32)640, (uint32)720, (int32)4, (uint32)0x000000FF);
	NkImage diffSame = NkImage::Create((uint32)640, (uint32)720, (int32)4, (uint32)0x000000FF);
	const DifferenceStats stereo = Soustraire(leftImage, rightImage, diff);
	const DifferenceStats forgotten = Soustraire(leftSame, rightSame, diffSame);
	diff.SavePNG(path("difference.png").c_str());
	diffSame.SavePNG(path("difference-vue-oubliee.png").c_str());
	std::remove(path("oeil-gauche-sans-decalage.png").c_str());
	std::remove(path("oeil-droit-sans-decalage.png").c_str());
	std::FILE *out = std::fopen(path("difference.txt").c_str(), "w");
	if (out) {
		std::fprintf(out, "deux yeux : %u pixels sur 460800, ecart max %u\n",
			stereo.pixels, stereo.maximum);
		std::fprintf(out, "vue oubliee : %u pixels sur 460800, ecart max %u\n",
			forgotten.pixels, forgotten.maximum);
		std::fclose(out);
	}
	std::printf("deux yeux : %u pixels sur 460800, ecart max %u\n", stereo.pixels, stereo.maximum);
	std::printf("vue oubliee : %u pixels sur 460800, ecart max %u\n", forgotten.pixels, forgotten.maximum);
	return out ? 0 : 1;
}
