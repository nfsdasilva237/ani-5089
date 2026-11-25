#include <iostream>
#include <vector>

int main() {
    int images = 0;
    int n = 0;
    if (!(std::cin >> images >> n)) return 0;
    std::vector<int> rend(n);
    for (int &dessine : rend) std::cin >> dessine;
    int index = 0;
    int rendues = 0;
    for (int i = 0; i < n; ++i) {
        if (rend[i] != 0) {
            std::cout << i + 1 << " OUI 1 " << index << '\n';
            ++rendues;
            if (images > 0) index = (index + 1) % images;
        } else {
            std::cout << i + 1 << " NON 0 -\n";
        }
    }
    std::cout << "RENDUES " << rendues << "\nFINS " << n << '\n';
}
