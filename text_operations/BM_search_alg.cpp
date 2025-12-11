#include<stdio.h>
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include "../text_operations/BM_search_alg.h"


std::map<char, int> shift_table_BM(std::string word){
    int n = word.length();
    std::map<char, int> table;

     for(int c = 0; c < 256; c++) {
        table[static_cast<char>(c)] = -1;
    }
    
    for(int i = 0; i < n - 1; i++){
        char key = word[i];
        table[key] =  i;
    }

    char last_key = word[n-1];
    if(table[last_key] == -1){
        table[last_key] = n - 1;
    }
    return table;
}


std::vector<int> word_find_BM(const std::string word, const std::string text){
    std::vector<int> positions;
    int n = word.length();
    int m = text.length();

    if( m < n || word.empty() || text.empty() ){
        return positions;
    }

    std::map<char, int> table = shift_table_BM(word);

    int t = 0;   
    bool match = true;
    while (t <= m - n){
        int false_pos;
        for(int i = n-1; i>=0; i--){
            if (std::tolower(text[t + i]) != std::tolower(word[i])){
                match = false;
                false_pos = t + i;
                break;
            }
        }
        if (match){
            positions.push_back(t);
            t++;
        }  
        else{
            int shift;
            char key = std::tolower(text[false_pos]);
            int shift = false_pos - table[key];
            t += shift;
        }
    }
    return positions;

}
