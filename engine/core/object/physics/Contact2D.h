//
// (c) 2023 Eduardo Doria.
//

#ifndef CONTACT2D_H
#define CONTACT2D_H

class b2Contact;

namespace Supernova{

    class Contact2D{
    private:
        b2Contact* contact;

    public:
        Contact2D(b2Contact* contact);
        virtual ~Contact2D();

        Contact2D(const Contact2D& rhs);
        Contact2D& operator=(const Contact2D& rhs);
    };
}

#endif //CONTACT2D_H