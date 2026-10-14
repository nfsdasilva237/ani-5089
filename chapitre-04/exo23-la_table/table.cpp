mat4 plateau = Translate({0.60f, 0.725f, 0.70f}) * Scale({1.20f, 0.05f, 0.80f});
float xs[2] = {0.03f, 1.17f}, zs[2] = {0.33f, 1.07f};
for (float x : xs) for (float z : zs)
    pied = Translate({x, 0.35f, z}) * Scale({0.06f, 0.70f, 0.06f});