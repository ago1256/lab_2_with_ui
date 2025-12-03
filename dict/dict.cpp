#include "dict.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cctype>

Radix_tree global_dict;

bool init_global_dict() {
    if (global_dict.deserialize_from_binary("../data/dict.bin")) {
        std::cout << "Load words " << global_dict.size() << std::endl;
        return true;
    }
    
    /*
    std::cout << "Бинарный файл не найден. Построение словаря из текстового файла..." << std::endl;
    
    if (!global_dict.build_from_text_file("../data/dict.txt", "../data/dict.bin")) {
        errors_detection(Error::DICTIONARY_ERROR);
        return false;
    }*/
    
    return true;
}


void create_global_dict() {
    if (!init_global_dict()) {
        errors_detection(Error::DICTIONARY_ERROR);
    }
}

bool load_dictionary_from_file(const std::string& filename) {
    return global_dict.build_from_text_file(filename, "");
}

bool save_dictionary_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    auto words = global_dict.get_dict();
    for (const auto& word : words) {
        file << word << "\n";
    }
    
    file.close();
    return true;
}

bool save_dictionary_full(const std::string& text_file,
                         const std::string& binary_file) {
    if (!save_dictionary_to_file(text_file)) {
        return false;
    }
    return global_dict.serialize_to_binary(binary_file);
}

void test_insert() {
    std::cout << "Insert test: ";
    std::string new_word = "sdkvjahvla4783ivgveifvg";
    global_dict.insert(new_word);
    assert(global_dict.contains(new_word));
    std::cout << "pass\n";
}

void test_search() {
    std::cout << "Search test: ";
    std::string test_word = "aaaaa185816258215382135w";
    assert(!global_dict.contains(test_word));
    std::cout << "pass\n";
}

void test_remove() {
    std::cout << "Remove test: ";
    std::string word = "zoom";
    assert(global_dict.contains(word));
    global_dict.remove(word);
    assert(!global_dict.contains(word));
    global_dict.insert(word);
    std::cout << "pass\n";
}

void run_tests() {
    test_insert();
    test_search(); 
    test_remove();
}