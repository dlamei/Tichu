/*! \class UUID
    \brief Represents a universally unique identifier (UUID).
 
 The UUID class is used to generate and manipulate universally unique identifiers.
 It provides methods for comparison, retrieval of string representation, and checking for emptiness.
*/

/*! \class TichuException
    \brief Represents an exception.

    The TichuException class is derived from std::exception and provides a way to throw exceptions
    with Tichu-specific error messages.
*/

/*! \class MessageQueue
    \brief Thread-safe queue for message handling.

 The MessageQueue class provides a thread-safe queue implementation for handling messages.
 It supports pushing and popping items from the queue in a thread-safe manner.
*/

#ifndef TICHU_UTILS_H
#define TICHU_UTILS_H

#include <variant>
#include <utility>
#include <queue>
#include <functional>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*! \struct overloaded
    \brief Helper type for std::visit.

 The overloaded struct is a helper type for using std::visit with multiple lambdas or function objects.
 Visit [https://en.cppreference.com/w/cpp/utility/variant/visit] for more information.

 The explicit deduction guide is not needed as of C++20.
*/
template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace nlohmann {
    /**
     * \struct nlohmann::adl_serializer<std::optional<T>>
     * \brief Serialization definition for optional types.
    */
    template<typename T>
    struct adl_serializer<std::optional<T>> {
        static void from_json(const json &j, std::optional<T> &opt) {
            if (j.is_null()) {
                opt = std::nullopt;
            } else {
                opt = j.get<T>();
            }
        }

        static void to_json(json &json, std::optional<T> t) {
            if (t) {
                json = *t;
            } else {
                json = nullptr;
            }
        }
    };

    /**
     *  \struct nlohmann::adl_serializer<std::shared_ptr<T>>
     *  \brief Serialization definition for shared pointers.
    */
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json& j, const std::shared_ptr<T>& opt)
        {
            if (opt)
            {
                j = *opt;
            }
            else
            {
                j = nullptr;
            }
        }

        static void from_json(const json& j, std::shared_ptr<T>& opt)
        {
            if (j.is_null())
            {
                opt = nullptr;
            }
            else
            {
                opt.reset(new T(j.get<T>()));
            }
        }
    };

    /**
     *  \struct variant_switch
     *  \brief Helper struct for variant serialization.
     * 
     * This struct is templated with a size parameter `N` and is used for variant serialization. 
     * It provides an overloaded function operator that recursively searches for the correct index within the variant and 
     * performs the conversion. For more information, refer to [https://github.com/nlohmann/json/issues/1261].
    */ 
    template <std::size_t N>
    struct variant_switch
    {
        template <typename Variant>
        void operator()(int index, json const &value, Variant &v) const
        {
            if (index == N)
                v = value.get<std::variant_alternative_t<N, Variant>>();
            else
                variant_switch<N - 1>{}(index, value, v);
        }
    };

    /** 
     * \struct variant_switch<0>
     * \brief Specialization of the variant_switch struct for index 0.
     * 
     * This struct is specialized for index 0 and is used for variant serialization. 
     * It provides an overloaded function operator that converts a JSON value to a variant when the index is 0. 
     * For more information, refer to [https://github.com/nlohmann/json/issues/1261].
    */
    template <>
    struct variant_switch<0>
    {
        template <typename Variant>
        void operator()(int index, json const &value, Variant &v) const
        {
            if (index == 0)
                v = value.get<std::variant_alternative_t<0, Variant>>();
            else
            {
                throw std::runtime_error(
                        "while converting json to variant: invalid index");
            }
        }
    };

    /**
     * \struct nlohmann::adl_serializer<std::variant<Args...>>
     * \brief Serialization definition for variants.
    */ 
    template <typename ...Args>
    struct adl_serializer<std::variant<Args...>> {
        static void to_json(json &j, const std::variant<Args...> &opt) {
            std::visit([&](auto&& value) {
                j["index"] = opt.index();
                j["value"] = std::forward<decltype(value)>(value);
            }, opt);
        }

        static void from_json(json const& j, std::variant<Args...>& v)
        {
            auto const index = j.at("index").get<int>();
            variant_switch<sizeof...(Args) - 1>{}(index, j.at("value"), v);
        }
    };
}

// macro for serialization of empty structs (useful for variants)
#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(TYPE) \
    inline void to_json(nlohmann::json &nlohmann_json_j, const TYPE &nlohmann_json_t) {} \
    inline void from_json(const nlohmann::json &nlohmann_json_j, TYPE &nlohmann_json_t) {}

inline void init_logger() {
    spdlog::set_level(spdlog::level::trace);
}

#define INFO(...) ::spdlog::info(__VA_ARGS__)
#define WARN(...) ::spdlog::warn(__VA_ARGS__)
#ifdef ERROR
#undef ERROR
#endif
#define ERROR(...) ::spdlog::error(__VA_ARGS__)

#if NDEBUG
#define ASSERT(x, ...)
#define DEBUG(...)
#else
#define DEBUG(...) ::spdlog::debug(__VA_ARGS__)
#define ASSERT(x, ...) { if(!(x)) {ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
#endif

class UUID {
    std::string _id;

public:
    UUID() = default;

    explicit UUID(std::string id)
            : _id(std::move(id)) {}

    static UUID create();

    bool operator==(const UUID &other) const {
        return _id == other._id;
    };

    bool operator!=(const UUID &other) const {
        return !(*this == other);
    }

    [[nodiscard]] bool empty() const { return _id.empty(); }

    [[nodiscard]] const char *c_str() const { return _id.c_str(); }

    [[nodiscard]] const std::string &string() const { return _id; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UUID, _id)
};

/**
 *  \struct std::hash<UUID>
 * \brief Specialization of the std::hash template for UUID.
 * 
 * This specialization provides a hash function for the UUID class, allowing it to be used in unordered containers.
 * 
 *  The hash is generated based on the string representation of the UUID.
*/
template<>
struct std::hash<UUID> {
    std::size_t operator()(const UUID &id) const {
        return std::hash<std::string>()(id.string());
    }
};

class TichuException : public std::exception {
private:
    std::string _msg;
public:
    explicit TichuException(std::string message) : _msg(std::move(message)) {};

    [[nodiscard]] const char *what() const noexcept override {
        return _msg.c_str();
    }
};

// thread safe queue
template<typename T>
class MessageQueue {
public:

    void push(T item) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(item);
    }

    std::optional<T> try_pop() {
        std::unique_lock<std::mutex> lock(_mutex);

        if (_queue.empty()) {
            return {};
        } else {
            T res = _queue.back();
            _queue.pop();
            return res;
        }
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};



#endif //TICHU_UTILS_H
