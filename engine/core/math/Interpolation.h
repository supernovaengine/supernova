// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

namespace doriax {

    class Interpolation {
    public:
        // Cubic Hermite segment between keys k and k+1 (glTF 2.0 Appendix C,
        // CUBICSPLINE): t is the segment-normalized factor, td the segment
        // duration in seconds; bk is key k's out-tangent and ak1 is key k+1's
        // in-tangent (tangents are per-second, hence the td scaling). Works
        // for any T with operator+ and operator*(float) — Vector3, Quaternion,
        // float. Quaternion results must be normalized by the caller.
        template<typename T>
        static T cubicSpline(float t, float td, const T& vk, const T& bk, const T& vk1, const T& ak1){
            float t2 = t * t;
            float t3 = t2 * t;
            return vk * (2*t3 - 3*t2 + 1) + bk * (td * (t3 - 2*t2 + t)) + vk1 * (-2*t3 + 3*t2) + ak1 * (td * (t3 - t2));
        }
    };

}

#endif //INTERPOLATION_H
