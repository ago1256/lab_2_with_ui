#pragma once
#include<map>

std::string to_lower(const std::string& str);

std::map<char, int> shift_table_BMH(const std::string& word);
std::vector<int> word_find_BMH(const std::string& word, const std::string& text);
