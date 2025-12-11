#pragma once

#include "../radix_tree/radix_tree.h"
#include "../errors/errors.h"

extern Radix_tree global_dict;

bool init_global_dict();
bool save_dictionary_to_binary(const std::string& filename);
bool save_dictionary_to_text(const std::string& filename);
bool save_dictionary_full();