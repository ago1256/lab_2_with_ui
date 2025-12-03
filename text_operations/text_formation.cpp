#include "text_formation.h"
#include <algorithm>
#include <cctype>
#include <sstream>



Text_formation::Text_formation(size_t width, size_t lines) 
    : page_width(width), lines_per_page(lines) {
}

std::vector<std::string> Text_formation::format_to_pages(const std::string& text) const {
    std::vector<std::string> pages;
    
    if (text.empty()) return pages;

    std::vector<std::string> lines = split_to_lines(text);
    
    for (size_t i = 0; i < lines.size(); i += lines_per_page) {
        std::string page;
        
        size_t end = std::min(i + lines_per_page, lines.size());
        for (size_t j = i; j < end; ++j) {
            page += lines[j] + "\n";
        }
        
        if (!page.empty() && page.back() == '\n') {
            page.pop_back();
        }
        
        pages.push_back(page);
    }
    
    return pages;
}

std::vector<std::string> Text_formation::split_to_words(const std::string& text) const {
    std::vector<std::string> words;
    
    if (text.empty()) return words;
    
    size_t pos = 0;
    size_t length = text.length();
    
    while (pos < length) {
        while (pos < length && std::isspace(static_cast<unsigned char>(text[pos]))) {
            pos++;
        }
        if (pos >= length) break;
        
        size_t start_pos = pos;
        
        while (pos < length && !std::isspace(static_cast<unsigned char>(text[pos]))) {
            pos++;
        }
        
        std::string word = text.substr(start_pos, pos - start_pos);
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    return words;
}

std::vector<std::string> Text_formation::split_to_lines(const std::string& text) const {
    std::vector<std::string> lines;
    
    if (text.empty() || page_width == 0 || page_width < 10) {
        // Минимальная ширина
        lines.push_back(text);
        return lines;
    }
    
    std::vector<std::string> words = split_to_words(text);
    
    if (words.empty()) {
        lines.push_back("");
        return lines;
    }
    
    std::string current_line = words[0];
    
    for (size_t i = 1; i < words.size(); ++i) {
        const std::string& word = words[i];
        
        // Пробел + слово
        size_t new_length = current_line.length() + 1 + word.length();
        
        if (new_length <= page_width) {
            // Вмещается - добавляем с пробелом
            current_line += " " + word;
        } else {
            // Не вмещается - сохраняем текущую строку
            lines.push_back(current_line);
            
            // Начинаем новую строку с текущего слова
            current_line = word;
        }
    }
    
    // Добавляем последнюю строку
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    
    return lines;
}

size_t Text_formation::get_page_width() const {
    return page_width;
}

size_t Text_formation::get_lines_per_page() const {
    return lines_per_page;
}

