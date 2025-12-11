#pragma once
#include<map>
#include<string>
#include<vector>

std::map<char, int> shift_table_BM(std::string word);
std::vector<int> word_find_BM(const std::string word, const std::string text);