//
// (c) 2024 Eduardo Doria.
//

#ifndef b2ContactImpulse_h
#define b2ContactImpulse_h

#include <cstdint>

class b2ContactImpulse;

namespace Supernova{

    class ContactImpulse2D{
    private:
        const b2ContactImpulse* contactImpulse;

    public:
        ContactImpulse2D(const b2ContactImpulse* contactImpulse);
        virtual ~ContactImpulse2D();

        ContactImpulse2D(const ContactImpulse2D& rhs);
        ContactImpulse2D& operator=(const ContactImpulse2D& rhs);

        const b2ContactImpulse* getBox2DContactImpulse() const;

        int32_t getCount() const;
        float getNormalImpulses(int32_t index) const;
        float getTangentImpulses(int32_t index) const;
    };
}

#endif //b2ContactImpulse_h