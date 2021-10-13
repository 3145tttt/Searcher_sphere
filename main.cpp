#include <iostream>
#include <string>
#include <vector>
#include "IndexMaker.h"
#include "Searcher.h"


template<typename T>
void print_vector(std::vector<T> a){
    size_t n = a.size();
    for(size_t i = 0; i < n; ++i)
        std::cout << a[i] << " ";
    std::cout << "\n";
}

void print_top(std::vector<std::string> s, size_t n = 5){
    if(s.empty()){
        printf("Sorry, vector is empty :(\n");
        return;
    }

    for(size_t i = 0; i < s.size() && i < n; ++i){
        printf("%lu) %s\n", i + 1, s[i].c_str());
    }
    printf("\n");
}

int main(int argC, char *argV[]){

    if(argC > 1){
        printf("==============run tests=============\n");
        Searcher my_search("documents_id.txt", "url.txt", "dic.txt", true);
        my_search.make_index_full("./data/", "index.txt");
        printf("================TESTS===============\n");
        std::vector<std::string> tests = { "Путин ", "Обама ", "Яблоко ", "Яблоко | ( Путин & Обама ) " };
        for(size_t i = 0; i < tests.size(); ++i){
            printf("%lu) %s \n", i + 1, tests[i].c_str());
            printf("Naive search\n");
            print_top(my_search.search(tests[i], true));
            printf("Stream search\n");
            print_top(my_search.search(tests[i], false));
        }
        printf("====================================\n");
        my_search.printInfo();
        return 0;
    }

    bool compress;
    std::cout << "Нужно ли сжимать обратный индекс? 1 - да 0 - нет ";
    std::cin >> compress;
    std::cout << std::endl;

    bool full;
    std::cout << "Построить индекс с нуля? 1 - да 0 - нет ";
    std::cin >> full;
    std::cout << std::endl;

    Searcher my_search("documents_id.txt", "url.txt", "dic.txt", compress);
    if(full){
        my_search.make_index_full("./data/", "index.txt");
    } else {
        my_search.make_index_short("index.txt");
    }

    my_search.printInfo();
    
    std::string s;
    std::cout << "Введите свой запрос в формате \"token op token \" (с пробелом в конце запроса)\n";
    getline(std::cin, s);
    while(getline(std::cin, s)){
        print_top(my_search.search(s, false));
    }
    return 0;
}
