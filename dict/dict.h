#pragma once

#include "../radix_tree/radix_tree.h"
#include "../errors/errors.h"

extern Radix_tree global_dict;

bool init_global_dict();

void create_global_dict();
void test_insert();
void test_search();
void test_remove();
void run_tests();
bool load_dictionary_from_file(const std::string& filename);
bool save_dictionary_to_file(const std::string& filename);
bool save_dictionary_full(const std::string& text_file = "../data/dict.txt", const std::string& binary_file = "../data/dict.bin");