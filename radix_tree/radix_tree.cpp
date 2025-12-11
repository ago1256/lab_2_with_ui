#include "radix_tree.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cctype>

Radix_tree::Node::Node(const std::string& k, bool end) 
    : key(k), is_end(end), link(nullptr), next(nullptr) {}

Radix_tree::Node::~Node() {
    delete link;
    delete next;
}

void Radix_tree::Node::serialize(std::ofstream& out) const {
    size_t key_len = key.length();
    out.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
    out.write(key.c_str(), key_len);
    
    out.write(reinterpret_cast<const char*>(&is_end), sizeof(is_end));
    
    bool has_link = (link != nullptr);
    bool has_next = (next != nullptr);
    out.write(reinterpret_cast<const char*>(&has_link), sizeof(has_link));
    out.write(reinterpret_cast<const char*>(&has_next), sizeof(has_next));
    
    if (has_link) {
        link->serialize(out);
    }
    if (has_next) {
        next->serialize(out);
    }
}

Radix_tree::Node* Radix_tree::Node::deserialize(std::ifstream& in) {
    if (!in) return nullptr;
    
    size_t key_len;
    in.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
    if (!in) return nullptr;
    
    char* key_buf = new char[key_len + 1];
    in.read(key_buf, key_len);
    if (!in) {
        delete[] key_buf;
        return nullptr;
    }
    std::string key(key_buf, key_len);  
    delete[] key_buf;
    
    bool is_end;
    in.read(reinterpret_cast<char*>(&is_end), sizeof(is_end));
    if (!in) return nullptr;
    
    bool has_link, has_next;
    in.read(reinterpret_cast<char*>(&has_link), sizeof(has_link));
    in.read(reinterpret_cast<char*>(&has_next), sizeof(has_next));
    if (!in) return nullptr;
    
    Node* node = new Node(key, is_end);
    
    if (has_link) {
        node->link = deserialize(in);
    }
    if (has_next) {
        node->next = deserialize(in);
    }
    
    return node;
}

int Radix_tree::prefix(const std::string& str1, const std::string& str2) const {
    int min_len = std::min(str1.length(), str2.length());
    for (int i = 0; i < min_len; ++i) {
        if (str1[i] != str2[i]) return i;
    }
    return min_len;
}

Radix_tree::Node* Radix_tree::find(Node* t, const std::string& x, int n) const {
    if (n == -1) n = x.length();
    if (!t) return nullptr;
    
    int k = prefix(x, t->key);
    
    if (k == 0) return find(t->next, x, n);
    if (k == n && k == t->key.length() && t->is_end) return t;
    if (k == t->key.length()) return find(t->link, x.substr(k), n - k);
    
    return nullptr;
}

void Radix_tree::split(Node* t, int len) {
    std::string end_of_old_key = t->key.substr(len);
    Node* new_child = new Node(end_of_old_key, t->is_end);
    new_child->link = t->link;
    
    t->key = t->key.substr(0, len);
    t->is_end = false;
    t->link = new_child;
}

void Radix_tree::merge(Node* node) {
    if (!node->link) return;
    
    Node* child = node->link;
    node->key += child->key;
    node->is_end = child->is_end;
    node->link = child->link;
    
    child->link = nullptr;
    delete child;
}

void Radix_tree::get_words(Node* node, const std::string& prefix, std::vector<std::string>& words) const {
    if (!node) return;
    
    std::string current = prefix + node->key;
    if (node->is_end) {
        words.push_back(current);
    }
    
    get_words(node->link, current, words);
    get_words(node->next, prefix, words);
}

Radix_tree::Radix_tree() : root(nullptr), words_count(0) {}

Radix_tree::~Radix_tree() {
    clear();
}

bool Radix_tree::contains(const std::string& word) const {
    if (word.empty()) return false;
    std::string word_without_register = word;
    word_without_register[0] = std::tolower(word_without_register[0]);
    return find(root, word_without_register, -1) != nullptr;
}

void Radix_tree::insert(const std::string& word_with_reg) {
    std::string word = word_with_reg;
    word[0] = std::tolower(word[0]);
    if (word.empty() || contains(word)) return;
    
    auto insert_recursive = [this](Node* t, const std::string& x, auto&& self) -> Node* {
        if (!t) return new Node(x, true);
        
        int k = prefix(x, t->key);
        
        if (k == 0) {
            t->next = self(t->next, x, self);
        }
        else if (k == t->key.length()) {
            if (k == x.length()) {
                t->is_end = true;
            } else {
                t->link = self(t->link, x.substr(k), self);
            }
        }
        else if (k == x.length()) {
            split(t, k);
            t->is_end = true; 
        }
        else {
            split(t, k);
            t->link = self(t->link, x.substr(k), self);
        }
        
        return t;
    };
    
    root = insert_recursive(root, word, insert_recursive);
    words_count++;
}

void Radix_tree::remove(const std::string& word) {
    if (word.empty() || !contains(word)) return;
    
    auto remove_recursive = [this](Node* t, const std::string& x, auto&& self) -> Node* {
        if (!t) return nullptr;
        
        int k = prefix(x, t->key);
        
        if (k == 0) {
            t->next = self(t->next, x, self);
        }
        else if (k == x.length() && k == t->key.length() && t->is_end) {
            Node* next = t->next;
            t->next = nullptr;
            delete t;
            return next;
        }
        else if (k == t->key.length()) {
            t->link = self(t->link, x.substr(k), self);
            
            if (t->link && !t->link->next && !t->is_end) {
                merge(t);
            }
            
            if (!t->link && !t->is_end) {
                Node* next = t->next;
                t->next = nullptr;
                delete t;
                return next;
            }
        }
        
        return t;
    };
    
    root = remove_recursive(root, word, remove_recursive);
    words_count--;
}

std::vector<std::string> Radix_tree::get_dict() const {
    std::vector<std::string> words;
    get_words(root, "", words);
    return words;
}

int Radix_tree::size() const { 
    return words_count; 
}

bool Radix_tree::empty() const { 
    return words_count == 0; 
}

void Radix_tree::clear() {
    delete root;
    root = nullptr;
    words_count = 0;
}

bool Radix_tree::serialize_to_binary(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }
    
    out.write(reinterpret_cast<const char*>(&words_count), sizeof(words_count));
    
    if (root) {
        root->serialize(out);
    }
    
    out.close();
    return true;
}

bool Radix_tree::deserialize_from_binary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    
    clear();

    in.read(reinterpret_cast<char*>(&words_count), sizeof(words_count));
    if (!in) {
        clear();
        return false;
    }
    
    if (words_count > 0) {
        root = Node::deserialize(in);
        if (!root) {
            clear();
            return false;
        }
    }
    
    in.close();
    return true;
}

bool Radix_tree::build_from_text_file(const std::string& text_file, const std::string& binary_file) {
    std::ifstream in(text_file);
    if (!in.is_open()) {
        return false;
    }
    clear();
    std::string word;
    int count = 0;
    
    while (in >> word) {
        insert(word);
        count++;
    }
    
    in.close();

    if (!binary_file.empty()) {
        if (serialize_to_binary(binary_file)) {
            std::cout << "Save to bin file: " << binary_file << std::endl;
        } else {
            std::cout << "Error saving to bin file" << std::endl;
        }
    }
    
    return true;
}
