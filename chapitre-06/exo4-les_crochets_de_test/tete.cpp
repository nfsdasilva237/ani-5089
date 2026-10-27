#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct V3 { float x, y, z; };
struct Tete {
    float lacet = 0.f, tangage = 0.f;
    V3 position{0.f, 1.70f, 1.20f};
    V3 Regard() const {
        const float cp = std::cos(tangage);
        return {-std::sin(lacet) * cp, std::sin(tangage), -std::cos(lacet) * cp};
    }
    void PoserTete(float yawRad, float pitchRad, V3 p) {
        lacet = yawRad; tangage = pitchRad; position = p;
    }
};

static bool proche(float a, float b) { return std::fabs(a-b) < 1e-4f; }
static bool epreuve(const char *nom, Tete &t, float yaw, float pitch, V3 attendu) {
    t.PoserTete(yaw, pitch, {0.f, 1.70f, 1.20f});
    const V3 r=t.Regard();
    const bool ok=proche(r.x,attendu.x)&&proche(r.y,attendu.y)&&proche(r.z,attendu.z);
    std::printf("%s : attendu (%.3f, %.3f, %.3f), obtenu (%.3f, %.3f, %.3f), %s\n",
        nom, attendu.x,attendu.y,attendu.z,r.x,r.y,r.z,ok?"OK":"ECHEC");
    return ok;
}

int main() {
    Tete tete;
    const bool test = std::getenv("MASALLE_TEST") && std::strcmp(std::getenv("MASALLE_TEST"),"1")==0;
    if (!test) { std::puts("Définir MASALLE_TEST=1 pour lancer les tests déterministes."); return 0; }
    bool ok=true;
    ok &= epreuve("lacet 90, tangage 0",tete,1.570796327f,0.f,{-1.f,0.f,0.f});
    ok &= epreuve("lacet 0, tangage 30",tete,0.f,0.523598776f,{0.f,0.5f,-0.8660254f});
    ok &= epreuve("lacet 180, tangage 0",tete,3.141592654f,0.f,{0.f,0.f,1.f});
    return ok?0:1;
}
