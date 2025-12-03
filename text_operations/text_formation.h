#pragma once
#include <string>
#include <vector>

class Text_formation {
private:
    size_t page_width;  
    size_t lines_per_page; 
    
public:
    Text_formation(size_t width = 80, size_t lines = 30);
    
    std::vector<std::string> format_to_pages(const std::string& text) const;
    
    size_t get_page_width() const;
    size_t get_lines_per_page() const;
    
private:
    std::vector<std::string> split_to_lines(const std::string& text) const;
    
    std::string normalize_text(const std::string& text) const;
  
    std::vector<std::string> split_to_words(const std::string& text) const;
};
