// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <bitset>

namespace doriax {

    // 64 should be max number of components types
    using Signature = std::bitset<64>;

}

#endif //SIGNATURE_H