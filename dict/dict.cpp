#include "dict.h"
#include <iostream>
#include <fstream>

Radix_tree global_dict;

bool init_global_dict() {
    if (global_dict.deserialize_from_binary("../data/dict.bin")) {
        return true;
    }
    
    if (global_dict.build_from_text_file("../data/dict.txt", "../data/dict.bin")) {
        return true;
    }
    
    std::cerr << "ERROR: Failed to load dictionary\n";
    return false;
}


bool save_dictionary_to_text(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    auto words = global_dict.get_dict();
    for (const auto& word : words) {
        file << word << "\n";
        if (file.fail()) return false;
    }
    
    return true;
}

bool save_dictionary_to_binary(const std::string& filename) {
    return global_dict.serialize_to_binary(filename);
}

bool save_dictionary_full() {
    bool text_ok = save_dictionary_to_text("../data/dict.txt");
    bool bin_ok = save_dictionary_to_binary("../data/dict.bin");
    
    if (!text_ok) {
        std::cerr << "Failed to save text dictionary\n";
        return false;
    }
    
    if (!bin_ok) {
        std::cerr << "Failed to save bin dictionary\n";
    }
    
    return true;
}