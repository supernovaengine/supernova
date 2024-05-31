//
// (c) 2024 Eduardo Doria.
//

#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <bitset>

namespace Supernova {

    // 64 should be max number of components types
    using Signature = std::bitset<64>;

}

#endif //SIGNATURE_H