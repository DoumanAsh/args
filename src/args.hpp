#pragma once

#include <exception>
#include <tuple>
#include <type_traits>
#include <iterator>
#include <string_view>
#include <array>

namespace args {

namespace detail {

template<class Tuple, class Function, std::size_t... Index>
inline void tuple_for_each(std::index_sequence<Index...>, Tuple&& tuple, Function&& function) {
    struct monostate { };
    (void)(monostate[]){
        (void(function(std::get<Index>(std::forward<Tuple>(tuple)))),
        monostate())...
    };
}

template<typename Arg>
inline void on_arg_long(Arg&& arg) {
    if (arg.is_flag()) {
        arg.count += 1;
    }
}

} // detail

template<class Tuple, class Function>
inline void tuple_for_each(Tuple&& tuple, Function&& function) {
    return detail::tuple_for_each(std::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>(),
                                  std::forward<Tuple>(tuple), std::forward<Function>(function));
}

template <const char* name>
struct Missing: std::exception {
    Missing() noexcept = default;
    Missing(Missing&&) = delete;
    Missing(const Missing&) = delete;

    virtual const char* what() const noexcept {
        return name;
    }
};

struct Required {
};

struct NotRequired {
};

template <const char* LONG_NAME, const char* SHORT_NAME, std::size_t N = 1, typename required = NotRequired>
class Option {
    static_assert(std::is_same<required, NotRequired>::value || std::is_same<required, Required>::value,
                  "Invalid required. Must be either args::Required or args::NotRequired");

    std::size_t count;
    std::array<const char*, N> args;

    public:
        Option() noexcept = default;
        Option(Option&&) noexcept = default;
        Option(const Option&) = delete;

        inline constexpr const char* long_name() const noexcept {
            return LONG_NAME;
        }

        inline constexpr const char* short_name() const noexcept {
            return SHORT_NAME;
        }

        inline constexpr const char* is_flag() const noexcept {
            return N == 0;
        }

        ///Returns whether any value is captured
        inline constexpr bool has_value() const noexcept {
            return args[0] != nullptr;
        }

        ///Number of values captured
        inline constexpr std::size_t len() const noexcept {
            return this->count;
        }

        template <typename T = required>
        inline constexpr auto is_missing() const noexcept -> typename std::enable_if<std::is_same<T, NotRequired>::value, bool>::type {
            return false;
        }

        template <typename T = required>
        inline constexpr auto is_missing() const noexcept -> typename std::enable_if<std::is_same<T, Required>::value, bool>::type {
            return !this->has_value();
        }

        inline constexpr const char* value() noexcept(false) {
            if (this->has_value()) {
                return this->arg[0];
            } else {
                throw Missing<LONG_NAME>();
            }
        }
};

template <typename ... Arg>
class Args {
    std::tuple<Arg...> options = { Arg()... };

    public:
        Args() noexcept = default;
        Args(const Args&) = delete;

        ///Gets argument using `std::get`
        template <typename T>
        inline constexpr const T& get() const noexcept {
            return std::get<T>(this->options);
        }

        ///Returns number of options
        inline constexpr std::size_t size() const noexcept {
            return std::tuple_size<std::tuple<Arg...>>::value;
        }

        template <typename Iterator>
        inline constexpr bool parse(Iterator begin, Iterator end) const noexcept {
            static_assert(std::is_same<typename std::iterator_traits<Iterator>::value_type, const char*>(),
                         "Iterator must be over const char*");

            for (; begin != end; ++begin) {
                if (*begin == nullptr) {
                    break;
                } else {
                    std::string_view arg(*begin);
                    if (arg.empty()) {
                        continue;
                    }

                    const size_t type = arg.find_first_not_of('-');
                    if (type == std::string_view::npos) {
                        //non-option/non-flag
                    } else if (type == 1) {
                        arg.remove_prefix(type);

                        //`-` without name
                        if (arg.empty()) {
                            return false;
                        }
                    } else if (type == 2) {
                        arg.remove_prefix(type);

                        //`--` without name
                        if (arg.empty()) {
                            return false;
                        }

                        //TODO: tuple_for_each()
                    } else {
                        //too many `-`
                        return false;
                    }
                }
            }

            return true;
        }
};

} //namespace args
