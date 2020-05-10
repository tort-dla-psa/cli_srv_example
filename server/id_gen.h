#pragma once
#include <vector>
#include <string>
#include <random>
#include <algorithm>

class IdGenerator{
    std::vector<std::string> ids;

    std::string alphabet = "01234567890abcdef";
    size_t len;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> dist;
public:
    IdGenerator(const size_t& len=8,
        const std::string& alphabet = "01234567890abcdef")
    {
        this->len = len;
        this->alphabet = alphabet;
        gen = std::mt19937(rd());
        dist = std::uniform_int_distribution<unsigned>(0, alphabet.size()-1);
    }

    auto get_id(){
        while(true){
            std::string tmp_id(len, ' ');
            for(size_t i=0; i<len; i++){
                char ch = alphabet.at(dist(gen));
                tmp_id[i] = ch;
            }
            if(std::find(ids.begin(), ids.end(), tmp_id)!=ids.end()){
                continue;
            }else{
                ids.emplace_back(tmp_id);
                break;
            }
        }
        return ids.back();
    }
};