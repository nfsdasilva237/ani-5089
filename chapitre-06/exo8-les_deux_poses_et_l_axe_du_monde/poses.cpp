#include "NKMath/NkQuat.h"
#include "NKMath/NkVec.h"

#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;

static void ImprimerPose(const char *nom, const char *methode,
	const NkVec3f &tete, const NkVec3f &droite, float ipd, bool axeMonde) {
	const NkVec3f decalage = axeMonde ? NkVec3f(1.f, 0.f, 0.f) : droite;
	const float demi = ipd * 0.5f;
	const NkVec3f gauche = tete - decalage * demi;
	const NkVec3f droit = tete + decalage * demi;
	std::printf("%s, %s : G (%.4f, %.4f, %.4f) D (%.4f, %.4f, %.4f)\n",
		nom, methode, (double)gauche.x, (double)gauche.y, (double)gauche.z,
		(double)droit.x, (double)droit.y, (double)droit.z);
}

static float EcartMillimetres(const NkVec3f &a, const NkVec3f &b) {
	const float dx = a.x - b.x;
	const float dy = a.y - b.y;
	const float dz = a.z - b.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz) * 1000.f;
}

int main() {
	constexpr float ipd = 0.063f;
	const NkVec3f position(0.30f, 1.70f, 1.20f);
	const NkQuatf orientations[] = {
		NkQuatf::Identity(),
		NkQuatf::RotateZ(NkAngle(30.f)),
		NkQuatf::RotateY(NkAngle(90.f))
	};
	const char *noms[] = {"neutre", "roulis 30", "lacet 90"};

	for (int i = 0; i < 3; ++i) {
		const NkVec3f droiteTete = -orientations[i].Right();
		const NkVec3f gaucheTete = position - droiteTete * (ipd * 0.5f);
		const NkVec3f gaucheMonde = position - NkVec3f(1.f, 0.f, 0.f) * (ipd * 0.5f);
		ImprimerPose(noms[i], "tete", position, droiteTete, ipd, false);
		ImprimerPose(noms[i], "monde", position, droiteTete, ipd, true);
		std::printf("%s : ecart %.3f mm\n", noms[i],
			(double)EcartMillimetres(gaucheTete, gaucheMonde));
	}
	return 0;
}
