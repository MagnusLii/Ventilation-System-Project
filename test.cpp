#include "src/logHandler.cpp"
#include <iostream>

int main() {

    uint8_t arr1[8] = {1, 0, 0, 0, 0, 1, 91, 145};
    //uint8_t arr2[8] = {1 3 3 7 0 0 3 16};
    //uint8_t arr3[8] = {1 3 3 7 0 0 187 46};

    std::cout << getChecksum(arr1, 8) << std::endl;
    //std::cout << getChecksum(arr2, 8) << std::endl;
    //std::cout << getChecksum(arr3, 8) << std::endl;
    
    return 0;
}