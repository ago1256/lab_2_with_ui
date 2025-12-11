#include "../radix_tree/radix_tree.h"
#include "tests/test.h"
#include <iostream>
#include <string>


int main(int argc, char* argv[]) {

    std::string text_file = "../data/dict.txt";
    std::string binary_file = "../data/dict.bin";
    
    Radix_tree dictionary;
    
    std::cout << "Radix Tree: " << text_file << std::endl;
    std::cout << "Binary file: " << binary_file << std::endl;
    
    if (!dictionary.build_from_text_file(text_file, binary_file)) {
        std::cerr << "ERROR BUILDING DICT" << text_file << std::endl;
        return 1;
    }
    run_tests();
    std::cout << "Word count: " << dictionary.size() << std::endl;
    return 0;
}