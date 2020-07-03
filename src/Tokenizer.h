#ifndef _Tokenizer_H_
#define _Tokenizer_H_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Tokenizer
{
    public:
        Tokenizer();
        ~Tokenizer();
        vector<string> tokenize(const char input[], char devider);

    private:
};

#endif