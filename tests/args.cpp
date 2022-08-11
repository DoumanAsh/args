#include <catch.hpp>
#include <array>

#include "args.hpp"

constexpr static const char long_flag[] = "flag";
constexpr static const char short_flag[] = "f";
constexpr static const char long_opt[] = "opt";
constexpr static const char short_opt[] = "o";

using Flag = args::Option<long_flag, short_flag>;
using Opt = args::Option<long_opt, short_opt, 1, args::Required>;

TEST_CASE("it works") {
    constexpr args::Args<Flag, Opt> args;

    REQUIRE(args.size() == 2);

    const auto& flag = args.get<Flag>();
    const auto& opt = args.get<Opt>();

    REQUIRE(flag.len() == 0);
    REQUIRE(opt.len() == 0);
    REQUIRE(!flag.is_missing());
    REQUIRE(opt.is_missing());

    constexpr std::array<const char*, 2> input = {
        "--flag", "lolka"
    };
    constexpr auto result = args.parse(input.cbegin(), input.cend());

    REQUIRE(flag.len() == 1);
    REQUIRE(result);
    REQUIRE(!flag.is_missing());
}
