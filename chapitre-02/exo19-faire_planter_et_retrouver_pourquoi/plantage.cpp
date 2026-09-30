#include <cstdio>

int main() {
    std::printf("Plantage volontaire dans 3, 2, 1...\n");
    volatile int* p = nullptr;
    return *p;   
}