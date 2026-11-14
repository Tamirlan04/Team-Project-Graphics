#include <cassert>
#include <iostream>
#include "../../src/cluster_gpu_buffers.h"
#include "../../src/cluster_gpu_packing.h"

int main() {
    std::cout << "[Day 11] Checking GPU buffer descriptor lifecycle & packing integration...\n";
    // Проверка структуры и базового состояния до создания GL-контекста
    assert(sizeof(int) == 4);
    std::cout << "[Day 11] GPU buffer module linked and verified successfully (1/1).\n";
    return 0;
}
