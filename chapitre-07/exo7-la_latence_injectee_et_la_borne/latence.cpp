

















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
#include "NKMath/NkQuat.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <vector>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {





	const float32 kPiece = 4.00f;
	const float32 kPlafond = 2.50f;
	const float32 kMur = 0.10f;
	const float32 kPorteL = 0.90f;
	const float32 kPorteH = 2.00f;
	const float32 kFenL = 1.20f;
	const float32 kFenH = 1.00f;
	const float32 kFenY = 1.40f;
	const float32 kTableL = 1.20f;
	const float32 kTableP = 0.80f;
	const float32 kTableH = 0.75f;
	const float32 kYeux = 1.70f;

	const float32 kDemi = kPiece * 0.5f;



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

		{
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


		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);



		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);



		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);


		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);



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



		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

	struct PoseEchantillon {
		double temps;
		NkQuatf pose;
		NkVec3f vitesse;
	};

	double LacetGeste(double temps) {
		const double t = temps - 1.0;
		if (t <= 0.) return 0.;
		if (t >= .400) return 90.;
		return 45. * (1. - std::cos(3.14159265358979323846 * t / .400));
	}

	NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dt) {
		if (dt <= 0.f) return NkVec3f(0.f, 0.f, 0.f);
		NkQuatf delta = (to * from.Conjugate()).Normalized();
		if (delta.w < 0.f) delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
		const float32 angle = delta.Angle().Rad();
		if (angle <= 1e-6f) return NkVec3f(0.f, 0.f, 0.f);
		return delta.Axis() * (angle / dt);
	}

	NkQuatf Extrapoler(const NkQuatf &pose, const NkVec3f &vitesse, float32 dt) {
		const float32 speed = vitesse.Len();
		if (speed <= 1e-6f) return pose;
		const NkVec3f axis = vitesse * (1.f / speed);
		return (NkQuatf(NkAngle::FromRad(speed * dt), axis) * pose).Normalized();
	}

	double ErreurDeg(const NkQuatf &a, const NkQuatf &b) {
		return (b * a.Conjugate()).Normalized().Angle().Deg();
	}

	NkQuatf PoseA(double temps) {
		return NkQuatf::RotateY(NkAngle((float32)LacetGeste(temps))).Normalized();
	}

	PoseEchantillon EchantillonA(const std::vector<PoseEchantillon> &historique,
								 double cible) {
		PoseEchantillon choisi = historique.front();
		for (const PoseEchantillon &p : historique) {
			if (p.temps <= cible) choisi = p;
			else break;
		}
	return choisi;
	}

	void MesurerLatences() {
		const double periode = 1. / 72.;
		std::vector<PoseEchantillon> historique;
		const double latences[] = {0., .010, .020, .040, .080};
		double sommeAvec[5] = {}, sommeSans[5] = {}, maxAvec[5] = {}, maxSans[5] = {};
		size_t comptes[5] = {};
		for (unsigned i = 0; i <= 72 * 3; ++i) {
			const double t = i * periode;
			const NkQuatf pose = PoseA(t);
			NkVec3f vitesse(0.f, 0.f, 0.f);
			if (!historique.empty()) {
				vitesse = VitesseAngulaire(historique.back().pose, pose,
					(float32)(t - historique.back().temps));
			}
			historique.push_back({t, pose, vitesse});
			if (historique.size() > 64) historique.erase(historique.begin());
			if (t >= 1. && t <= 1.900001) {
				for (size_t j = 0; j < 5; ++j) {
					const PoseEchantillon e = EchantillonA(historique, t - latences[j]);
				const NkQuatf vraie = PoseA(t + periode);
				const float32 horizon = (float32)std::min(.1, t + periode - e.temps);
				const double avec = ErreurDeg(Extrapoler(e.pose, e.vitesse, horizon), vraie);
				const double sans = ErreurDeg(e.pose, vraie);
					sommeAvec[j] += avec; sommeSans[j] += sans;
					maxAvec[j] = std::max(maxAvec[j], avec);
					maxSans[j] = std::max(maxSans[j], sans); ++comptes[j];
				}
			}
		}
		for (size_t j = 0; j < 5; ++j)
			std::printf("latence %.0f ms : avec %.3f deg (max %.3f), sans %.3f deg (max %.3f)\n",
				latences[j] * 1000., sommeAvec[j] / comptes[j], maxAvec[j],
				sommeSans[j] / comptes[j], maxSans[j]);
		for (bool borne : {false, true}) {
			std::vector<PoseEchantillon> h;
			for (unsigned i = 0; i <= (unsigned)(1.2 / periode); ++i) {
				const double t = i * periode;
				const NkQuatf pose = PoseA(t);
				NkVec3f vitesse(0.f, 0.f, 0.f);
				if (!h.empty()) vitesse = VitesseAngulaire(h.back().pose, pose,
					(float32)(t - h.back().temps));
				h.push_back({t, pose, vitesse});
				if (h.size() > 64) h.erase(h.begin());
			}
			const PoseEchantillon e = EchantillonA(h, 1.2 - 1.0);
			const double horizon = 1.2 + periode - e.temps;
			const float32 dt = (float32)(borne ? std::min(.1, horizon) : horizon);
			const NkQuatf pred = Extrapoler(e.pose, e.vitesse, dt);
			const NkQuatf vraie = PoseA(1.2 + periode);
			const double degPred = 2. * std::atan2((double)pred.y, (double)pred.w) * 57.295779513;
			const double degVraie = 2. * std::atan2((double)vraie.y, (double)vraie.w) * 57.295779513;
			std::printf("%s : echantillon a %.3f s, horizon %.3f ms, predit %.3f deg, vrai %.3f deg\n",
				borne ? "avec borne" : "sans borne", e.temps, dt * 1000., degPred, degVraie);
		}
	}

}

int nkmain(const NkEntryState &state) {
	(void)state;
	MesurerLatences();
	return 0;

	NkWindowConfig cfg;
	cfg.title = "Ma salle";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;

	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
	if (device == nullptr) {
		std::printf("[MaSalle] aucune API graphique disponible\n");
		return 1;
	}



















	NkRendererConfig cfgEcran =
		NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize()) {
		std::printf("[MaSalle] le renderer d'ecran n'a pas pu s'initialiser\n");
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkOffscreenDesc od;
	od.width = devInit.width;
	od.height = devInit.height;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = true;
	od.name = "MaSalle_Cible";

	NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
	if (cible == nullptr) {
		std::printf("[MaSalle] cible hors ecran refusee\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkRendererConfig cfgSalle =
		NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
	cfgSalle.Disable(NK_SS_RENDER2D);
	cfgSalle.Disable(NK_SS_TEXT);
	cfgSalle.Disable(NK_SS_OVERLAY);
	cfgSalle.Enable(NK_SS_OFFSCREEN);




	if (EnvU64("MASALLE_POST", 1) == 0) {
		cfgSalle.Disable(NK_SS_POST_PROCESS);
	}

	NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
	if (renderer == nullptr || !renderer->Initialize()) {
		std::printf("[MaSalle] le renderer de la salle n'a pas pu s'initialiser\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}
	renderer->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

	std::printf("[MaSalle] salle de %.1f m sur %.1f m, plafond a %.2f m\n",
				(double)kPiece, (double)kPiece, (double)kPlafond);



	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
	const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);


	uint64 image = 0;
	bool tourne = true;

	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) {
				tourne = false;
			}
		}
		if (!tourne) break;

		if (rEcran->BeginFrame()) {
			NkICommandBuffer *cmd = rEcran->GetCmd();

			NkRender3D *r3d = renderer->GetRender3D();




			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *mc = renderer->GetMaterialCollection()) {
				mc->Upload();
			}

			NkCamera3DData cd;


			cd.position = { 0.30f, kYeux, 1.20f };
			cd.target = { -0.70f, 1.10f, -kDemi };
			cd.up = { 0.f, 1.f, 0.f };
			cd.fovY = 70.f;
			cd.aspect = (float32)devInit.width / (float32)devInit.height;
			cd.nearPlane = 0.05f;
			cd.farPlane = 100.f;

			NkSceneContext sctx;
			sctx.camera = NkCamera3D(cd);
			sctx.time = (float32)image / 60.f;

			r3d->SetIBLStrength(0.35f);

			NkLightDesc soleil;
			soleil.type = NkLightType::NK_DIRECTIONAL;
			soleil.direction = { -0.5f, -1.f, -0.35f };
			soleil.color = { 1.f, 0.96f, 0.88f };
			soleil.intensity = 3.f;
			soleil.castShadow = false;
			sctx.lights.PushBack(soleil);

			r3d->BeginScene(sctx);
			SoumettreLaSalle(r3d, renderer->GetMeshSystem());



			if (auto *graph = renderer->GetRenderGraph()) {
				graph->Execute(cmd);
			}



			if (auto *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cible->GetColorResult(),
							   { 0.f, 0.f, (float32)devInit.width,
								 (float32)devInit.height });
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;



		if (imageDeCapture != 0 && image == imageDeCapture) {
			const char *chemin = getenv("MASALLE_SHOT_PATH");
			if (chemin == nullptr || *chemin == '\0') {
				chemin = "masalle.png";
			}
			const bool ok = cible->Capture(chemin);
			std::printf("[MaSalle] capture image %llu -> %s (%s)\n",
						(unsigned long long)image, chemin, ok ? "ok" : "echec");
		}

		if (imageDeSortie != 0 && image >= imageDeSortie) {
			tourne = false;
		}
	}

	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
