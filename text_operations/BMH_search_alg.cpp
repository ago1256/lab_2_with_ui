#include<stdio.h>
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include "../text_operations/BMH_search_alg.h"


std::string to_lower(const std::string& str) {
    std::string result;
    for(char c: str){
        result += std::tolower(c);
    }
    return result;
}

std::map<char, int> shift_table_BMH(const std::string& word){
    std::map<char, int> table;
    int n = word.length();
    
    for(int c = 0; c < 256; c++) {
        table[static_cast<char>(c)] = n;
    }
    
    std::string lower_word = to_lower(word);
    
    for(int i = 1; i < n; i++){
        char key = lower_word[n-i-1];
        if(table[key] == n) { 
            table[key] = i;
        }
    }
    
    char last_key = lower_word[n-1];
    if(table[last_key] == n) {
        table[last_key] = n;
    }

    return table;
}

std::vector<int> word_find_BMH(const std::string& word, const std::string& text){
    std::vector<int> positions;
    int n = word.length();
    int m = text.length();

    if(m < n || word.empty() || text.empty()){
        return positions;
    }

    std::map<char, int> table = shift_table_BMH(word);
    int j = 0;
    
    while (j <= m - n){
        bool match = true;
        
        for(int i = n-1; i >= 0; i--){
            if (std::tolower(text[j + i]) != std::tolower(word[i])){
                match = false;
                break;
            }
        }
        
        if (match){
            positions.push_back(j);
            j++;  
        }
        else{
            char key = std::tolower(text[j + n - 1]);
            int shift = table[key];
            j += shift;
        }
    }
    
    return positions;
}