#include "../dict/dict.h"
#include <iostream>
#include <cassert>
#include <map>
#include "test.h"

void test_insert() {
    std::cout << "Insert test: ";
    std::string new_word = "testword123";
    global_dict.insert(new_word);
    assert(global_dict.contains(new_word));
    std::cout << "pass\n";
}

void test_search() {
    std::cout << "Search test: ";
    std::string test_word = "nonexistentword456";
    assert(!global_dict.contains(test_word));
    std::cout << "pass\n";
}

void test_remove() {
    std::cout << "Remove test: ";
    std::string word = "zoom";
    global_dict.insert(word);
    assert(global_dict.contains(word));
    global_dict.remove(word);
    assert(!global_dict.contains(word));
    global_dict.insert(word); 
    std::cout << "pass\n";
}

void test_serialization() {
    std::cout << "Serialization test: ";
    
    Radix_tree temp_dict;
    temp_dict.insert("dvdvsdv");
    temp_dict.insert("tAASACA");
    temp_dict.insert("tttttt");
    
    assert(temp_dict.serialize_to_binary("test_dict.bin"));
    
    Radix_tree loaded_dict;
    assert(loaded_dict.deserialize_from_binary("test_dict.bin"));
    
    assert(loaded_dict.contains("dvdvsdv"));
    assert(loaded_dict.contains("tAASACA"));
    assert(loaded_dict.contains("tttttt"));
    assert(loaded_dict.size() == 3);
    
    std::remove("test_dict.bin");
    std::cout << "pass\n";
}

void test_edge_cases() {
    std::cout << "Edge cases test: ";
    
    assert(!global_dict.contains(""));
    
    std::string long_word(100, 'a');
    global_dict.insert(long_word);
    assert(global_dict.contains(long_word));
    global_dict.remove(long_word);
    
    std::string word = "don't";
    global_dict.insert(word);
    assert(global_dict.contains(word));
    
    std::cout << "pass\n"; 
}


void test_edge_cases_BMH() {
    std::cout << "Edge cases BMH test: ";
    {
        std::vector<int> result = word_find_BMH("", "zdhthetherh");
        assert(result.empty());
        
        result = word_find_BMH("word", "");
        assert(result.empty());
        
        result = word_find_BMH("", "");
        assert(result.empty());
    }
    
    {
        std::string long_word(100, 'a');
        std::string text = "ssssssssss" + long_word + "bbbbbbb";
        std::vector<int> result = word_find_BMH(long_word, text);
        assert(result.size() == 1 && result[0] == 10);
    }
    
    {
        std::string word = "AbAbvbaba";
        std::vector<int> result = word_find_BMH(word, word);
        assert(result.size() == 1 && result[0] == 0);
    }
    
    {
        std::string text = "a b c a b c";
        std::vector<int> result = word_find_BMH("a", text);
        assert(result.size() == 2 && result[0] == 0 && result[1] == 6);
    }
    std::cout << "pass\n";
}

void test_BMH() {
    std::cout << "BMH test: ";
    
    {
        std::string text = "Hello hELLO HELLO hello";
        std::string word = "hello";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 4);
        assert(result[0] == 0 && result[1] == 6 && result[2] == 12 && result[3] == 18);
    }

    {
        std::string text = "HeLlO WoRlD";
        std::string word = "hElLo";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 1 && result[0] == 0);
    }

    {
        std::string text = "carcda carrcarcsca carqwsdvwv";
        std::string word = "car";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 4);
        assert(result[0] == 0 && result[1] == 7 && result[2] == 11 && result[3] == 19);
    }
    
    {
        std::string text = "aaaaaa";
        std::string word = "aaa";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 4); 
    }
    
    {
        std::string text = "strtwtwtwtwe";
        std::string word = "s";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 1 && result[0] == 0);
        
        word = "e";
        result = word_find_BMH(word, text);
        assert(result.size() == 1 && result[0] == 11);
    }
    
    {
        std::string text = "word    word   word";
        std::string word = "word";
        std::vector<int> result = word_find_BMH(word, text);
        assert(result.size() == 3);
    }
    
    std::cout << "pass\n";
}


void test_shift_table_BMH() {
    std::cout << "Shift table test BMH: ";
    
    {
        std::map<char, int> table = shift_table_BMH("hello");
        assert(table['h'] == 4);
        assert(table['e'] == 3);
        assert(table['l'] == 1); 
        assert(table['o'] == 5); 
    }
    
    {
        std::map<char, int> table = shift_table_BMH("test");
        assert(table['t'] == 3);
        assert(table['e'] == 2);
        assert(table['s'] == 1);
    }
    
    {
        std::map<char, int> table = shift_table_BMH("a");
        assert(table['a'] == 1);
    }
    
    {
        std::map<char, int> table = shift_table_BMH("aaaa");
        assert(table['a'] == 1); 
    }
    
    std::cout << "pass\n";
}


void run_tests() {
    test_insert();
    test_search(); 
    test_remove();
    test_serialization();
    test_edge_cases();
    test_BMH();
    test_edge_cases_BMH();
    test_shift_table_BMH();
}

