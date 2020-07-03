#include "Tokenizer.h"

// constructor
Tokenizer::Tokenizer()
{

}

// destructor
Tokenizer::~Tokenizer()
{

}

// char array tokenizer
vector<string> Tokenizer::tokenize(const char input[], char devider)
{
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, devider))
    {
        tokens.push_back(token);
    }
    return tokens;
}
