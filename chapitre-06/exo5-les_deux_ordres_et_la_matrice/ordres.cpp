#include "NKMath/NKMath.h"

#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;

static void AfficherRegard(const char *etiquette, const NkVec3f &regard) {
	std::printf("%s : (%.4f, %.4f, %.4f)\n",
				etiquette, regard.x, regard.y, regard.z);
}

int main() {
	const float32 lacetDeg = 45.f;
	const float32 tangageDeg = 30.f;
	const NkVec3f avant(0.f, 0.f, -1.f);

	const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
	const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(tangageDeg));
	AfficherRegard("lacet*tangage", (qLacet * qTangage) * avant);
	AfficherRegard("tangage*lacet", (qTangage * qLacet) * avant);

	const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
							 NkMat4f::RotationX(NkAngle(tangageDeg));
	const NkQuatf qMatrice = NkQuatf(rotation).Normalized();
	AfficherRegard("matrices", qMatrice * avant);

	// Une elevation de 30 degres donne y = sin(30 degres) = 0.5.
	// Le lacet autour de Y ne change pas cette composante verticale.
	return 0;
}
