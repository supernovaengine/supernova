//
// (c) 2020 Eduardo Doria.
//

#include "UniqueToken.h"
#include <cstdlib>

using namespace Supernova;

int UniqueToken::id;

std::string UniqueToken::randString(const int len){
    char rand_id[len];
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i) {
        rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    rand_id[len] = 0;

    return std::string(rand_id);
}

std::string UniqueToken::get(){

    return randString(10) + std::to_string(id++);
}
