#include "NKTime/NkChrono.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"

#include <cstdio>
#include <cmath>

using namespace nkentseu;
using namespace nkentseu::math;

static double MesurerMatrices(float &somme) {
	NkChrono chrono;
	somme = 0.f;
	for (uint32 i = 0; i < 1000000; ++i) {
		const float lacetDeg = (i % 3600) * 0.1f;
		const float tangageDeg = (i % 1800) * 0.1f;
		const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
			NkMat4f::RotationX(NkAngle(tangageDeg));
		const NkQuatf q = NkQuatf(rotation).Normalized();
		somme += q.x + q.y + q.z + q.w;
	}
	return chrono.Elapsed().ToMilliseconds();
}

static double MesurerQuaternions(float &somme) {
	NkChrono chrono;
	somme = 0.f;
	for (uint32 i = 0; i < 1000000; ++i) {
		const float lacetDeg = (i % 3600) * 0.1f;
		const float tangageDeg = (i % 1800) * 0.1f;
		const NkQuatf q = (NkQuatf::RotateY(NkAngle(lacetDeg)) *
			NkQuatf::RotateX(NkAngle(tangageDeg))).Normalized();
		somme += q.x + q.y + q.z + q.w;
	}
	return chrono.Elapsed().ToMilliseconds();
}

int main() {
	const float lacetDeg = 37.f;
	const float tangageDeg = 21.f;
	const NkQuatf matrice = NkQuatf(
		NkMat4f::RotationY(NkAngle(lacetDeg)) *
		NkMat4f::RotationX(NkAngle(tangageDeg))).Normalized();
	const NkQuatf direct = (NkQuatf::RotateY(NkAngle(lacetDeg)) *
		NkQuatf::RotateX(NkAngle(tangageDeg))).Normalized();
	const float dx = matrice.x - direct.x;
	const float dy = matrice.y - direct.y;
	const float dz = matrice.z - direct.z;
	const float dw = matrice.w - direct.w;
	const float ecart = std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
	std::printf("verification quaternion : ecart %.8f\n", (double)ecart);

	for (int repetition = 1; repetition <= 3; ++repetition) {
		float sommeMatrices = 0.f;
		const double matricesMs = MesurerMatrices(sommeMatrices);
		float sommeQuaternions = 0.f;
		const double quaternionsMs = MesurerQuaternions(sommeQuaternions);
		std::printf("rep %d, matrices : %.3f ms, somme %.6f\n",
			repetition, matricesMs, (double)sommeMatrices);
		std::printf("rep %d, quaternions : %.3f ms, somme %.6f\n",
			repetition, quaternionsMs, (double)sommeQuaternions);
	}
	return ecart < 0.001f ? 0 : 1;
}
