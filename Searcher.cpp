#include "Searcher.h"


Searcher::Searcher(const std::string id_path, const std::string url_path, const std::string dic_path, bool compress){
    my_maker = IndexMaker(id_path, url_path, dic_path, compress);
}

void Searcher::make_index_full(const std::string data, const std::string result){
    unsigned start = clock();
    printf("Make reverse index from files (full) start!\n");
    my_maker.make_reverse_index_full(data, result);
    printf("Make reverse index from files (full) end!\n");
    unsigned end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time total: %lf\n", seconds);
}

void Searcher::make_index_short(const std::string result){
    unsigned start = clock();
    printf("Make reverse index from files (short) start!\n");
    my_maker.make_reverse_index_short(result);
    printf("Make reverse index from files (short) end!\n");
    unsigned end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time total: %lf\n", seconds);
}
void Searcher::printInfo(){
    my_maker.print_info();
}

std::vector<std::string> Searcher::search(const std::string str, const bool naive_f){
    unsigned start = clock();
    std::vector<std::string> words = split_request(str);

    if(words.empty())
        return {};
    std::vector<size_t> id_urls;
    if(naive_f){
        size_t t = 0;
        id_urls = naive_search(t, words);
    } else {
        id_urls = stream_search(words);
    }

    unsigned end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time total: %lf\n", seconds);

    if(id_urls.empty())
        return {};

    return my_maker.get_urls(id_urls);
}

std::vector<size_t> Searcher::naive_search(size_t& pos, std::vector<std::string> words){
    std::vector<size_t> answer = my_maker.get_urls_by_word(words[pos]);
    const size_t N = words.size();
    for(size_t i = pos + 1; i < N; ++i){
        if(words[i] == "&"){
            ++i;
            if(words[i] == "(") {
                ++i;
                answer = intersect(answer, naive_search(i, words));
            } else
                answer = intersect(answer, my_maker.get_urls_by_word(words[i]));
        } else if(words[i] == "|"){
            ++i;
            if(words[i] == "(") {
                ++i;
                answer = concate(answer, naive_search(i, words));
            } else
                answer = concate(answer, my_maker.get_urls_by_word(words[i]));
        } else if(words[i] == ")"){
            pos = i;
            return answer;    
        }else {
            printf("DEBUG: ERROR NAIVE SEARCH FORMAT %s\n", words[i].c_str());
            return {};
        }
    }
    return answer;
}

std::vector<size_t> Searcher::intersect(const std::vector<size_t> arr1, const std::vector<size_t> arr2){
    const size_t N1 = arr1.size(), N2 = arr2.size();
    std::vector<size_t> answer;
    size_t i1 = 0, i2 = 0;
    while(i1 < N1 && i2 < N2){
        if(arr1[i1] < arr2[i2])
            ++i1;
        else if(arr1[i1] > arr2[i2])
            ++i2;
        else {
            answer.push_back(arr1[i1]);
            ++i1;
            ++i2;
        }
    }

    return answer;
}

std::vector<size_t> Searcher::concate(const std::vector<size_t> arr1, const std::vector<size_t> arr2){
    const size_t N1 = arr1.size(), N2 = arr2.size();
    std::vector<size_t> answer = {};

    size_t i1 = 0, i2 = 0, last_push = 0;
    while(i1 < N1 || i2 < N2){
        if(i1 < N1 && i2 < N2){
            if(arr1[i1] < arr2[i2]){
                last_push = arr1[i1];
                ++i1;
            } else if(arr1[i1] > arr2[i2]){
                last_push = arr2[i2];
                ++i2;
            } else {
                if(last_push != arr1[i1])
                    last_push = arr1[i1];
                ++i1;
                ++i2;
            }
        } else if(i1 < N1){
            last_push = arr1[i1];
            ++i1;
        } else {
            last_push = arr2[i2];
            ++i2;
        }
        answer.push_back(last_push);
    }

    return answer;
}

std::vector<std::string> Searcher::split_request(const std::string str){
    std::string my_str = str;
    std::vector<std::string> answer;
    const std::string SEP  = " ";
    size_t pos = 0;
    while((pos = my_str.find(SEP)) != std::string::npos){
        answer.push_back(my_str.substr(0, pos));
        my_str.erase(0, pos + 1);
    }
    return answer;
}

inline int Searcher::check_word(std::string s) {
    return s != "|" && s != "&" && s != "(";
}

inline int Searcher::operation_code(std::string s) {
    return s == "&" ? AND_CODE : OR_CODE;
}

Node* Searcher::make_tree(std::vector<std::string> a, long long l, long long r){

    if((size_t) l >= a.size())
        return nullptr;
    if (l == r) {
            std::vector<size_t> urls = my_maker.get_urls_by_word(a[l]);
            if(urls.empty())
                return nullptr;
        Node *head = new Node{0, urls, nullptr, nullptr};
        return head;
    }
    int i = l;

    while (i < r && check_word(a[i])) {
        i++;
    }
    if (a[i] == "(") {
        int balanc_brackets = 1, j = i + 1;
        while (balanc_brackets != 0) {
            balanc_brackets += (a[j] == "(");
            balanc_brackets -= (a[j] == ")");
            j++;
        }
        --j;
        
        if(j + 1 <= r) {
            Node *right = make_tree(a, j, r);
            Node *left = make_tree(a, i + 1 , j - 1);
            return new Node(operation_code(a[j + 1]), {}, left, right);
        }
        return make_tree(a, i + 1, j - 1);
    }
        Node *right = make_tree(a, i + 1, r);
        Node *left = make_tree(a, l, i - 1);
        return new Node (operation_code(a[i]), {}, left, right);

}


size_t Searcher::binsearch(std::vector<size_t> a, size_t target) {
    if (target > a[a.size() - 1]) {
        return 0;
    }
    const size_t N = a.size();
    size_t l = 0, r = N;
    while (l + 1 != r) {
        size_t m = (l + r) / 2;
        if (a[m] > target) {
            r = m;
        } else if (a[m] < target) {
            l = m;
        } else {
            return a[m];
        }
    }
    return target > a[l] ? a[r] : a[l];
}

size_t Searcher::step(Node *node, unsigned id){
    if(node->op_code == 0)
        return binsearch(node->urls, id);

    if(!node->left || !node->right)
        return 0;
    
    size_t ID1 = step(node->left, id), ID2 = step(node->right, id);
    if(node->op_code == AND_CODE){
        while(ID1 != ID2 && ID2 != 0 && ID1 != 0){
            if(ID1 > ID2)
                ID2 = step(node->right, ID1);
            else
                ID1 = step(node->left, ID2);
        }
        if(ID1 == 0 || ID2 == 0)
            return 0;
        return ID1;
    }
    if(ID1 * ID2 == 0)
        return ID1 + ID2;
    return std::min(ID1, ID2);
}


std::vector<size_t> Searcher::stream_search(std::vector<std::string> words){
    Node *head = make_tree(words, 0, words.size() - 1);
    std::vector<size_t> ans;
    unsigned id = 1;
    while(1){
        id = step(head, id);
        if(id == 0)
            break;
        else
            ans.push_back(id++);
    }
    return ans;
}