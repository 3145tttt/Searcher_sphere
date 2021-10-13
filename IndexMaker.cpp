#include "IndexMaker.h"


IndexMaker::IndexMaker(const std::string id_path, const std::string url_path, const std::string dic_path, bool compress){
    _id_path = id_path;
    _url_path = url_path;
    _dic_path = dic_path;
    compress_flag = compress;
}

IndexMaker::IndexMaker(){}

void IndexMaker::make_reverse_index_full(const std::string data, const std::string result){
    make_id_file(data, _id_path);
    urls_to_file(_url_path);
    words_to_file(_dic_path);
    make_reverse_index_from_file(_id_path);
    reverse_index_to_file(result);

    if(compress_flag)
        make_compress_index();
}

void IndexMaker::make_reverse_index_short(const std::string result){
    make_urls_from_file(_url_path);
    make_dict_from_file(_dic_path);
    make_reverse_index_from_file(_id_path);
    reverse_index_to_file(result);

    if(compress_flag)
        make_compress_index();
}

std::vector<size_t> IndexMaker::get_urls_by_word(const std::string word){
    if(dictionary.contains(word)){
        if (!compress_flag)
            return reverse_index[dictionary[word]];

        return compressor.decompress(reverse_index_compress[dictionary[word]]);
    }
    return {};
}

std::vector<std::string> IndexMaker::get_urls(const std::vector<size_t> ids){
    const size_t n = ids.size();
    std::vector<std::string> ans(n);
    for(size_t i = 0; i < n; ++i){
        ans[i] = urls[ids[i]];
    }
    return ans;
}


void IndexMaker::print_info(){
    printf("=================INFO================\n");
    printf("Index have %lu urls\n", url_id);
    printf("Index have %lu words\n", word_id);

    size_t size = 0;
    for(size_t i = 0; i < word_id; ++i)
        size += reverse_index[i].size() * sizeof(size_t);

    printf("Index size is %lu bytes\n", size);

    if(compress_flag){
        size = 0;
        for(size_t i = 0; i < word_id; ++i)
            size += reverse_index_compress[i].size() * sizeof(unsigned char);
        printf("Index have compressed Index\n");
        printf("Compressed Index size is %lu bytes\n", size);
    }
    printf("=====================================\n \n");
}

void IndexMaker::urls_to_file(const std::string path){
    std::ofstream out(path, std::ios::out);
    for(const auto it : urls){
        out << it.first << " " << it.second << "\n";
    }
    out.close();
}

void IndexMaker::words_to_file(const std::string path){
    std::ofstream out(path, std::ios::out);
    for(const auto it : dictionary){
        out << it.first << " " << it.second << "\n";
    }
    out.close();
}

void IndexMaker::make_id_file(const std::string dir, const std::string path){
    std::ofstream index_file(path, std::ios::out); 
    size_t current_url = 0;
    for (const auto & entry : std::filesystem::directory_iterator(dir)){
        std::ifstream in(entry.path(), std::ios::in);
        std::string t;
        while(in >> t){
            const size_t pos = t.find("http");
            if(pos != std::string::npos){
                t = get_url_clean(t, pos);
                urls[url_id] = t;
                index_file << "\n" << 0 << " " << url_id << " ";
                current_url = url_id;
                ++url_id;
            } else {
                if(!current_url)
                    continue;
                
                if(!dictionary.contains(t)){
                    dictionary[t] = word_id;
                    ++word_id;
                }

                index_file << dictionary[t] << " ";
            }
        }
        in.close();
    }

    index_file.close();
}

void IndexMaker::make_reverse_index_from_file(const std::string path){
    std::ifstream in(path, std::ios::in);
    
    size_t t, cur_id = 0;
    std::unordered_set<size_t> bucket;
    while(in >> t){
        if(t == 0){
            if(cur_id){
                for(auto el : bucket){
                    reverse_index[el].push_back(cur_id);
                }
            }
            bucket.clear();
            in >> cur_id;
        } else {
            bucket.insert(t);
        }
    }
    in.close();
}

void IndexMaker::make_urls_from_file(const std::string path){
    std::ifstream in(path, std::ios::in);
    size_t id;
    while(in >> id){
        in >> urls[id];
        ++url_id;
    }
    in.close();
}

void IndexMaker::make_dict_from_file(const std::string path){
    std::ifstream in(path, std::ios::in);
    std::string word;
    while(in >> word){
        in >> dictionary[word];
        ++word_id;
    }
    in.close();
}

void IndexMaker::reverse_index_to_file(const std::string path){
    std::ofstream index_out(path, std::ios::out);
    for(size_t i = 1; i < word_id; ++i){
        size_t n = reverse_index[i].size();
        index_out << i << " ";
        for(size_t j = 0; j < n; ++j){
            index_out << reverse_index[i][j] << " ";
        }
        index_out << "\n";
    }
    index_out.close();
}


void IndexMaker::make_compress_index(){
    for(size_t i = 1; i < word_id; ++i)
        reverse_index_compress[i] = compressor.compress(reverse_index[i]); 
}

std::string IndexMaker::get_url_clean(std::string str, size_t right_edge){
    const size_t n = str.size();
    for(size_t i = right_edge; i < n; ++i){
        str[i - right_edge] = str[i];
        if(str[i] == 26){
            str[i - right_edge] = '\0';
            str.resize(i - right_edge);
            break;
        }
    }
    return str;
}