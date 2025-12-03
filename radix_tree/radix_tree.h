#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>

class Radix_tree {
private:
    struct Node {
        std::string key;   
        bool is_end; 
        Node* link;  // ребенок узла
        Node* next;  // альтернативные пути
        
        Node(const std::string& k, bool end = false);
        ~Node();
        
        // Методы для сериализации
        void serialize(std::ofstream& out) const;
        static Node* deserialize(std::ifstream& in);
    };
    
    Node* root;
    int words_count;

    // Приватные методы
    int prefix(const std::string& str1, const std::string& str2) const;
    Node* find(Node* t, const std::string& x, int n) const;
    void split(Node* t, int len);
    void merge(Node* node);
    void get_words(Node* node, const std::string& prefix, std::vector<std::string>& words) const;

public:
    Radix_tree();
    ~Radix_tree();

    bool contains(const std::string& word) const;
    void insert(const std::string& word);
    void remove(const std::string& word);
    std::vector<std::string> get_dict() const;
    int size() const;
    bool empty() const;
    void clear();
    
    // Новые методы для сериализации/десериализации
    bool serialize_to_binary(const std::string& filename) const;
    bool deserialize_from_binary(const std::string& filename);
    
    // Метод для построения из текстового файла с возможностью сохранения в бинарный
    bool build_from_text_file(const std::string& text_file, 
                             const std::string& binary_file = "");
};