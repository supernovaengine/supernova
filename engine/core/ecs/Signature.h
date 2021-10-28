#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <bitset>

namespace Supernova {

    // 32 should be max number of components types
    using Signature = std::bitset<32>;

}

#endif //SIGNATURE_H