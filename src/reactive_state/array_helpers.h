//
// Created by Manuel on 03.03.2021.
//

#ifndef LAMA_ARRAY_HELPERS_H
#define LAMA_ARRAY_HELPERS_H

namespace array_helpers {

    template<class T, class B> struct derived_from {
        static void constraints(T* p) { B* pb = p; }
        derived_from() { void(*p)(T*) = constraints; }
    };

}


#endif //LAMA_ARRAY_HELPERS_H
