#include <cstdio>

struct Vec3 {
    float x, y, z;
};

struct Quat {
    float x, y, z, w; 
    Vec3 Applique(const Vec3& v) const {
        Vec3 qv{x, y, z};
        Vec3 t{
            qv.y * v.z - qv.z * v.y,
            qv.z * v.x - qv.x * v.z,
            qv.x * v.y - qv.y * v.x
        };
        t = {t.x * 2.0f, t.y * 2.0f, t.z * 2.0f};

        Vec3 tt{
            qv.y * t.z - qv.z * t.y,
            qv.z * t.x - qv.x * t.z,
            qv.x * t.y - qv.y * t.x
        };

        return {
            v.x + t.x * w + tt.x,
            v.y + t.y * w + tt.y,
            v.z + t.z * w + tt.z
        };
    }
};

struct Pose {
    Vec3 position;
    Quat orientation;
    Vec3 Applique(const Vec3& p_entite) const {
        Vec3 tourne = orientation.Applique(p_entite);
        return {
            tourne.x + position.x,
            tourne.y + position.y,
            tourne.z + position.z
        };
    }
};

int main() {
    Pose pose;
    std::scanf("%f %f %f", &pose.position.x, &pose.position.y, &pose.position.z);
    std::scanf("%f %f %f %f", &pose.orientation.x, &pose.orientation.y,
                &pose.orientation.z, &pose.orientation.w);

    Vec3 point;
    std::scanf("%f %f %f", &point.x, &point.y, &point.z);

    Vec3 resultat = pose.Applique(point);

    std::printf("%.4f %.4f %.4f\n", resultat.x, resultat.y, resultat.z);

    return 0;
}