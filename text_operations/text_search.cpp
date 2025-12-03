#include "text_search.h"
#include <cctype>
#include <algorithm>


std::vector<int> Text_search::find_word(const std::string& text, const std::string& word) {
    std::vector<int> positions;
    
    if (text.empty() || word.empty() || word.length() > text.length()) {
        return positions;
    }
    
    for (int i = 0; i <= text.length() - word.length(); ++i) {
        bool match = true;
        for (int j = 0; j < word.length(); ++j) {
            if (text[i + j] != word[j]) {
                match = false;
                break;
            }
        }
        
        if (match) {
            positions.push_back(i);
        }
    }
    
    return positions;
}

std::vector<int> Text_search::find_word_without_register(const std::string& text, const std::string& word) {
    std::vector<int> positions;
    
    if (text.empty() || word.empty() || word.length() > text.length()) {
        return positions;
    }
    
    for (int i = 0; i <= text.length() - word.length(); ++i) {
        bool match = true;
        for (int j = 0; j < word.length(); ++j) {
            if (to_lower(text[i + j]) != to_lower(word[j])) {
                match = false;
                break;
            }
        }
        
        if (match) {
            positions.push_back(i);
        }
    }
    
    return positions;
}

char Text_search::to_lower(char ch) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}
