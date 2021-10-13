#pragma once
#include <vector>
#include <string>
#include "IndexMaker.h"

enum {
    AND_CODE = 1,
    OR_CODE = 2
};

//Node of stream search
struct Node
{
    Node *left;
    Node *right;
    std::vector<size_t> urls;
    char op_code;

    Node(char op, std::vector<size_t> a, Node *l, Node *r) {
        left = l;
        right = r;
        urls = a;
        op_code = op;
    }
};


class Searcher {

public:
    Searcher(const std::string id_path, const std::string url_path, const std::string dic_path, bool compress);
    
    //make index from data
    void make_index_full(const std::string data, const std::string result);

    //make index from url dictionary files
    void make_index_short(const std::string result);
    
    //main info about reverse index
    void printInfo();

    std::vector<std::string> search(const std::string str, const bool naive_f);

private:
    std::vector<size_t> naive_search(size_t& pos, std::vector<std::string> words);

    std::vector<size_t> stream_search(std::vector<std::string> words);
    
    //vector && vector
    std::vector<size_t> intersect(const std::vector<size_t> arr1, const std::vector<size_t> arr2);

    //vector || vector
    std::vector<size_t> concate(const std::vector<size_t> arr1, const std::vector<size_t> arr2);

    //split line to term
    std::vector<std::string> split_request(const std::string str);

    //for stream search
    inline int check_word(std::string s);

    inline int operation_code(std::string s);

    Node* make_tree(std::vector<std::string> a, long long l, long long r);

    size_t binsearch(std::vector<size_t> a, size_t target);

    size_t step(Node *node, unsigned id);

    IndexMaker my_maker;

};