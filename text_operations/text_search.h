#pragma once

#include <string>
#include <vector>
#include <cstddef>

class Text_search {

private:
    static char to_lower(char ch);

public:

    static std::vector<int> find_word(const std::string& text, const std::string& substring);
    
    static std::vector<int> find_word_without_register(const std::string& text, const std::string& substring);
};

