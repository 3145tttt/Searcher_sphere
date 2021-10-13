#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdio.h>

class VarByteCompressor {

public:
std::vector<unsigned char> compress(std::vector<size_t> arr) {
    if(arr.empty())
        return {};
    std::vector<unsigned char> ans;
    const size_t N = arr.size();
    std::vector<size_t> diff(N);
    diff[0] = arr[0];
    for(size_t i = 1; i < N; ++i)
        diff[i] = arr[i] - arr[i - 1];

    for(size_t i = 0; i < N; ++i){
        size_t cur = diff[i];
        while(cur >= MAX_VALUE){
            ans.push_back(cur & MASK);
            cur >>= 7;
        }

        unsigned char t = 0x80;
        t |= cur & MASK;
        ans.push_back(t);
    }
    return ans;
}

std::vector<size_t> decompress(const std::vector<unsigned char> arr) {
    std::vector<size_t> ans;
    size_t shift = 0, temp = 0;
    const size_t N = arr.size();
    for(size_t i = 0; i < N; ++i){
        size_t cur = arr[i];
        if(cur >= MAX_VALUE){
            temp |= (cur & MASK) << (7 * shift);
            shift = 0;
            if(ans.empty())
                ans.push_back(temp);
            else
                ans.push_back(temp + ans.back());
            temp = 0;
        } else {
            temp |= cur << (8 * shift);
            ++shift;
        }
    }
    return ans;
}

private:
    static const unsigned int MASK = 0x7f;
    static const unsigned int MAX_VALUE = 128;
};


class IndexMaker {

public:
    IndexMaker(const std::string id_path, const std::string url_path, const std::string dic_path, bool compress);

    IndexMaker();

    void make_reverse_index_full(const std::string data, const std::string result);

    void make_reverse_index_short(const std::string result);

    std::vector<size_t> get_urls_by_word(const std::string word);

    std::vector<std::string> get_urls(const std::vector<size_t> ids);

    void print_info();

private:
    void urls_to_file(const std::string path);

    void words_to_file(const std::string path);

    void make_id_file(const std::string dir, const std::string path);

    void make_reverse_index_from_file(const std::string path);

    void make_urls_from_file(const std::string path);

    void make_dict_from_file(const std::string path);

    void reverse_index_to_file(const std::string path);
    
    void make_compress_index();

    std::string get_url_clean(std::string str, size_t right_edge);


    std::string _id_path;
    std::string _url_path;
    std::string _dic_path;

    size_t url_id = 1, word_id = 1;
    std::unordered_map<std::string, size_t> dictionary;
    std::unordered_map<size_t, std::string> urls;


    std::unordered_map<size_t, std::vector<size_t>> reverse_index;

    VarByteCompressor compressor;
    bool compress_flag = true;
    std::unordered_map<size_t, std::vector<unsigned char>> reverse_index_compress;
};