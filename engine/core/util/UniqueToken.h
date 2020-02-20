//
// (c) 2020 Eduardo Doria.
//

#ifndef UNIQUETOKEN_H
#define UNIQUETOKEN_H

#include <string>

namespace Supernova {

    class UniqueToken {
    private:
        static int id;

    public:
        static std::string get();

    };

}

#endif //UNIQUETOKEN_H
