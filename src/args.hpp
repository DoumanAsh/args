#pragma once

#include <exception>
#include <tuple>
#include <type_traits>
#include <iterator>

namespace args {

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

template <const char* LONG_NAME, const char* SHORT_NAME, typename required = NotRequired>
class Option {
    static_assert(std::is_same<required, NotRequired>::value || std::is_same<required, Required>::value,
                  "Invalid required. Must be either args::Required or args::NotRequired");

    const char* arg = nullptr;

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

        inline constexpr bool has_value() const noexcept {
            return arg != nullptr;
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
                return this->arg;
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

        template <typename T>
        inline constexpr const T& get() const noexcept {
            return std::get<T>(this->options);
        }

        inline constexpr std::size_t size() const noexcept {
            return std::tuple_size<std::tuple<Arg...>>::value;
        }

        template <typename Iterator>
        inline constexpr bool parse(Iterator begin, Iterator end) noexcept {
            static_assert(std::is_same<typename std::iterator_traits<Iterator>::value_type, const char*>(),
                         "Iterator must be over const char*");

            for (; begin != end; ++begin) {
                if (*begin == nullptr) {
                    break;
                } else if (*begin[0] == '-') {
                    const size_t len = strlen(*begin);
                }
            }

            return true;
        }
};

} //namespace args
