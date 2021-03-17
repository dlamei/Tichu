//
// Created by Manuel on 03.03.2021.
//

#ifndef LAMA_ARRAY_HELPERS_H
#define LAMA_ARRAY_HELPERS_H

#include "diffs/array_diff.h"

namespace array_helpers {

    template<class T, class B> struct derived_from {
        static void constraints(T* p) { B* pb = p; }
        derived_from() { void(*p)(T*) = constraints; }
    };

    template<class T>
    /* WARNING: can only be called with vectors for type T that derives from "reactive_object" */
    static std::vector<T*> vector_from_diff(const array_diff* arr_diff){
        derived_from<T, reactive_object>(); // ensure T derives from reactive_object

        std::vector<T*> res;
        if (arr_diff != nullptr) {
            for (auto & arr_elem : arr_diff->get_diffs()) {
                switch (arr_elem->get_type()) {
                    case (array_elem_diff::add): {
                        try {
                            res.insert(res.begin() + arr_elem->get_index(), T::from_diff(arr_elem->get_diff()));
                        } catch (const std::exception& e) {
                            std::cerr << e.what() << std::endl;
                        }
                        break;
                    }
                    case (array_elem_diff::rem): {
                        res.erase(res.begin() + arr_elem->get_index());
                        break;
                    }
                    case (array_elem_diff::mod): {
                        res.at(arr_elem->get_index())->apply_diff_specialized(arr_elem->get_diff());
                        break;
                    }
                }
            }
        }

        return res;
    }


    template<class T>
    /* WARNING: can only be called with vectors for type T that derives from "reactive_object" */
    static bool apply_diff(std::vector<T*>& to_modify, const array_diff* arr_diff){
        derived_from<T, reactive_object>(); // ensure T derives from reactive_object
        bool has_changed = false;
        if (arr_diff != nullptr) {
            for (auto & arr_elem : arr_diff->get_diffs()) {
                switch (arr_elem->get_type()) {
                    case (array_elem_diff::add): {
                        try {
                            to_modify.insert(to_modify.begin() + arr_elem->get_index(), T::from_diff(arr_elem->get_diff()));
                            has_changed = true;
                            // TODO invoke elem added callback
                        } catch (const std::exception& e) {
                            std::cerr << e.what() << std::endl;
                        }
                        break;
                    }
                    case (array_elem_diff::rem): {
                        to_modify.erase(to_modify.begin() + arr_elem->get_index());
                        has_changed = true;
                        // TODO invoke elem removed callback
                        break;
                    }
                    case (array_elem_diff::mod): {
                        has_changed |= to_modify.at(arr_elem->get_index())->apply_diff_specialized(arr_elem->get_diff());
                        // TODO invoke elem modified callback (needed??)
                        break;
                    }
                }
            }
        }

        return has_changed;
    }
}


#endif //LAMA_ARRAY_HELPERS_H
