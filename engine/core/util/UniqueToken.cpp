//
// (c) 2020 Eduardo Doria.
//

#include "UniqueToken.h"
#include <cstdlib>

using namespace Supernova;

int UniqueToken::id;

std::string UniqueToken::get(){
    char rand_id[10];
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 10; ++i) {
        rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    id++;

    return std::string(rand_id) + std::to_string(id);
}
